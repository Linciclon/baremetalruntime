/** 
 * Bao, a Lightweight Static Partitioning Hypervisor 
 *
 * Copyright (c) Bao Project (www.bao-project.org), 2019-
 *
 * Authors:
 *      Jose Martins <jose.martins@bao-project.org>
 *      Sandro Pinto <sandro.pinto@bao-project.org>
 *
 * Bao is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation, with a special exception exempting guest code from such
 * license. See the COPYING file in the top-level directory for details. 
 *
 */

#include <core.h>
#include <util.h>
#include <stdlib.h>
#include <stdio.h>
#include <cpu.h>
#include <wfi.h>
#include <spinlock.h>
#include <plat.h>
#include <irq.h>
#include <systick.h>
#include <uart.h>
#include <timer.h>

#define TIMER_INTERVAL (TIME_S(1))

#define NUM_TICKS_FOR_BAD_ACCESS 20
#define BAD_ADDR (int *) 0x20031000

#define SAU_BASE                 0xE000EDD0UL
#define SAU_CTRL_ENABLE          (1UL << 0UL)
#define SAU_REGION_ENABLE        (1UL << 0UL)
#define SAU_REGION_NSC           (1UL << 1UL)

#define DWT_CTRL                 (*(volatile uint32_t *)0xE0001000UL)
#define DWT_CYCCNT               (*(volatile uint32_t *)0xE0001004UL)
#define DEMCR                    (*(volatile uint32_t *)0xE000EDFCUL)
#define DEMCR_TRCENA             (1UL << 24UL)
#define DWT_CTRL_CYCCNTENA       (1UL << 0UL)

#define SCB_NS_BASE              0xE002ED00UL
//#define SCB_NS_VTOR              (*(volatile uint32_t *)(SCB_NS_BASE + SCB_VTOR_OFF))
#define SCB_NS_VTOR              (*(volatile uint32_t *)(0xE002ED08UL))

uint8_t Tick_Counter = 0;

#ifdef SECURE_INTERRUPT_LATENCY
volatile uint32_t secure_systick_entry_cvr;
volatile uint32_t secure_systick_latency_cycles;
volatile uint32_t secure_systick_sample_count;
#endif
struct sau {
    volatile uint32_t ctrl;
    volatile const uint32_t type;
    volatile uint32_t rnr;
    volatile uint32_t rbar;
    volatile uint32_t rlar;
};

static struct sau *const sau = (struct sau *)SAU_BASE;

static void sau_set_nonsecure_region(uint32_t region, uint32_t base,
                                     uint32_t size)
{
    sau->rnr = region;
    sau->rbar = base & 0xFFFFFFE0UL;
    sau->rlar = ((base + size - 1UL) & 0xFFFFFFE0UL) | SAU_REGION_ENABLE;
}

static void sau_set_nsc_region(uint32_t region, uint32_t base, uint32_t size)
{
    sau->rnr = region;
    sau->rbar = base & 0xFFFFFFE0UL;
    sau->rlar = ((base + size - 1UL) & 0xFFFFFFE0UL) |
                SAU_REGION_NSC | SAU_REGION_ENABLE;
}

static void dwt_enable_cycle_counter(void)
{
    DEMCR |= DEMCR_TRCENA;
    DWT_CYCCNT = 0;
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;
    asm volatile("dsb\nisb" ::: "memory");
}

//function that is called from secure_dwt_ns_to_s_probe to stall secure world and force constant secure wolrd execution
void stall_secure_world(){
    while(1);
}

__attribute__((naked, section(".nsc_gateway"), aligned(32), used))
uint32_t secure_dwt_ns_to_s_probe(void)
{
    __asm volatile(
        "sg\n"
        "ldr r0, =0xE0001004\n"
        "ldr r0, [r0]\n"
#ifdef NON_SECURE_WORLD_INT_LAT
        "b stall_secure_world\n" //non-secure interrupt latency measurment 
#endif
        "bxns lr\n");
}

static void secure_app_deinit(void)
{
    asm volatile("cpsid i" ::: "memory");

    for (uint32_t i = 0; i < 16; i++) {
        nvic->icer[i] = 0xFFFeFFFFUL;
        nvic->icpr[i] = 0xFFFeFFFFUL;
        nvic->itns[i] = 0xFFFeFFFFUL;
    }
}

static void configure_nonsecure_world(void)
{
    sau_set_nonsecure_region(0, PLAT_NS_CODE_MEM_BASE, PLAT_NS_CODE_MEM_SIZE);
    sau_set_nonsecure_region(1, PLAT_NS_MEM_BASE, PLAT_NS_MEM_SIZE);
    sau_set_nsc_region(2, PLAT_NSC_BASE, PLAT_NSC_SIZE);
    //sau_set_nonsecure_region(2, PLAT_NS_UART_ADDR, PLAT_NS_UART_SIZE);
    sau_set_nonsecure_region(3, 0x40000000, 0x10000000); //perifericos
    //sau_set_nonsecure_region(4, 0x9FC00u, 0x400); //frerertos

    sau->ctrl = SAU_CTRL_ENABLE;

    asm volatile("dsb\nisb" ::: "memory");
    asm volatile("cpsie i" ::: "memory");
}

static void jump_to_nonsecure(void)
{
    uint32_t const *ns_vector_table = (uint32_t const *)PLAT_NS_VECTOR_TABLE_BASE;
    uint32_t ns_msp = ns_vector_table[0];
    uint32_t ns_reset = ns_vector_table[1]; //also try this one

    //clear bit Thumb to ns_reset
    ns_reset &= ~1UL;

    secure_app_deinit();
    configure_nonsecure_world();
    dwt_enable_cycle_counter();

    SCB_NS_VTOR = PLAT_NS_VECTOR_TABLE_BASE;
    asm volatile(
        "msr msp_ns, %0\n"
        "dsb\n"
        "isb\n"
        "ldr r2, =0xE0001004\n"
        "movs r3, #0\n"
        "str r3, [r2]\n"
        "bxns %1\n"
        :
        : "r"(ns_msp), "r"(ns_reset)
        : "r2", "r3", "memory");

    __builtin_unreachable();
}

void uart_rx_handler()
{
    uart_clear_rxirq();
    printf("Secure App: UART RX Handler\n");
}

void timer_handler()
{
#ifdef SECURE_INTERRUPT_LATENCY
    uint32_t latency_ns = (uint32_t)(((uint64_t)secure_systick_latency_cycles *
                                      1000000000ULL) / TIMER_FREQ);

    printf("Secure SysTick latency: %u cycles, %u ns (%u CVR, sample %u)\n",
           secure_systick_latency_cycles,
           latency_ns,
           secure_systick_entry_cvr,
           secure_systick_sample_count);
#endif
    // int *ptr_addr = BAD_ADDR;
    // Tick_Counter++;
    // if(Tick_Counter == NUM_TICKS_FOR_BAD_ACCESS)
    // {
    //     printf("Memory Access ADDR:%d\n", &ptr_addr);
    //     /*Bad Memory Access here*/
    //     *ptr_addr = 0xdead; 
    // }
}

void main(void)
{
    printf("#############################\n");
    printf("This is the secure Baremetal! \n");
    printf("#############################\n");

    irq_set_handler(UART_IRQ_ID, uart_rx_handler);
    irq_set_handler(TIMER_IRQ_ID, timer_handler);

    uart_enable_rxirq();

    // Tick_Counter = 0; 

    irq_enable(UART_IRQ_ID);
    irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);

#ifdef SECURE_INTERRUPT_LATENCY
    irq_enable(TIMER_IRQ_ID);
    irq_set_prio(TIMER_IRQ_ID, IRQ_MAX_PRIO);
    timer_set(TIMER_INTERVAL - 1);
    systick->cvr = 0;
#endif

    //while(1);
    //Jump to Non-secure
    jump_to_nonsecure();

    while(1) wfi();
}
