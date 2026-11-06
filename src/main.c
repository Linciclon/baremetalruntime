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
#include <uart.h>
#include <timer.h>

#ifndef PLAT_RZT2M
#include <systick.h>
#ifdef USINGTIMER
#include <lpc_timer0.h>
#endif
#endif

#ifdef SECURE_INTERRUPT_LATENCY
volatile uint32_t secure_systick_entry_cvr;
volatile uint32_t secure_systick_latency_cycles;
volatile uint32_t secure_systick_sample_count;
volatile uint32_t dwt_cycles_at_interrupt_entry;
#endif

#if defined(SECURE_INTERRUPT_LATENCY) && defined(USINGTIMER)
#define N_WARMUP_SAMPLES 100U
#define N_SAMPLES        1000U

spinlock_t print_lock = SPINLOCK_INITVAL;
unsigned long samples[N_SAMPLES];

static void collect_timer_latency_sample(void)
{
    static unsigned long warmup_cnt = N_WARMUP_SAMPLES;
    static unsigned long sample_cnt;

    if (warmup_cnt != 0U) {
        warmup_cnt--;
        #ifdef SECURE_INTERRUPT_LATENCY
            #ifdef RUN_INSIDE_VM
                #ifdef EXECUTION_IN_NONSECURE_CONTEXT
                    crosshyp_hypercall(CROSSCON_HC_END_INT, 0, 0); //notify hypervisor that the secure interrupt has been handled
                #endif
            #endif
        #endif
        return;
    }

    if (sample_cnt >= N_SAMPLES) {
        #ifdef SECURE_INTERRUPT_LATENCY
            #ifdef RUN_INSIDE_VM
                #ifdef EXECUTION_IN_NONSECURE_CONTEXT
                    crosshyp_hypercall(CROSSCON_HC_END_INT, 0, 0); //notify hypervisor that the secure interrupt has been handled
                #endif
            #endif
        #endif
        return;
    }

    samples[sample_cnt++] = (unsigned long)secure_systick_latency_cycles;

    if (sample_cnt == N_SAMPLES) {
        /* Stop first: printing must not perturb any sample in this batch. */
        timer_disable();

        spin_lock(&print_lock);
        for (size_t i = 0U; i < N_SAMPLES; i++) {
#ifdef PLAT_RZT2M
#ifdef RUN_INSIDE_VM
            printf("%u,ARMv8-R,MTU%d,active_vm,%lu\n",
                   (unsigned int)(i + 1U), MTU_CHANNEL, samples[i]);
#else
            printf("%u,ARMv8-R,MTU%d,native,%lu\n",
                   (unsigned int)(i + 1U), MTU_CHANNEL, samples[i]);
#endif
#else
#ifdef RUN_INSIDE_VM
            printf("%u,ARMv8-M,CTIMER0,active_vm,%lu\n",
                   (unsigned int)(i + 1U), samples[i]);
#else
            printf("%u,ARMv8-M,CTIMER0,native,%lu\n",
                   (unsigned int)(i + 1U), samples[i]);
#endif
#endif
        }
        spin_unlock(&print_lock);
    }
}
#endif

#define BAO_IMAGE_START          0x10000000UL
#define BAO_HC_OFF               0x41UL
#define BAO_HC_ADDR              (BAO_IMAGE_START + BAO_HC_OFF)
#define CROSSCON_HC_BOOT         0xf
#define CROSSCON_HC_END_INT      0xe

void (*crosshyp_hypercall)(unsigned int, unsigned int, unsigned int) =
    (void (*)(unsigned int, unsigned int, unsigned int))BAO_HC_ADDR;

#ifndef PLAT_RZT2M

#define TIMER_INTERVAL (TIME_US(10))
//#define TIMER_INTERVAL (142857142)

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
    //DWT_CTRL |= DWT_CTRL_CYCCNTENA;
    //asm volatile("dsb\nisb" ::: "memory");
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
#ifdef NONSECURE_INTERRUPT_LATENCY
        "b stall_secure_world\n" //non-secure interrupt latency measurment 
#endif
        "bxns lr\n");
}

#define SECURE_IRQ_MASK \
    ((1UL << 16) | (1UL << CTIMER0_PERIPH_IRQ_ID))


#ifndef RUN_INSIDE_VM
static void secure_app_deinit(void)
{
    asm volatile("cpsid i" ::: "memory");

    for (uint32_t i = 0; i < 16; i++) {
        uint32_t deinit_mask = 0xFFFEFFFFUL;

#if defined(SECURE_INTERRUPT_LATENCY) && \
    defined(EXECUTION_IN_NONSECURE_CONTEXT) && defined(USINGTIMER)
    #ifndef RUN_INSIDE_VM
        if (i == (CTIMER0_PERIPH_IRQ_ID >> 5U)) {
            deinit_mask &= ~SECURE_IRQ_MASK;
        }
    #endif
#endif

        nvic->icer[i] = deinit_mask;
        nvic->icpr[i] = deinit_mask;
        nvic->itns[i] = deinit_mask;
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
#endif

static void jump_to_nonsecure(void)
{
#ifndef RUN_INSIDE_VM
    uint32_t const *ns_vector_table = (uint32_t const *)PLAT_NS_VECTOR_TABLE_BASE;
    uint32_t ns_msp = ns_vector_table[0];
    uint32_t ns_reset = ns_vector_table[1]; //also try this one

    //clear bit Thumb to ns_reset
    ns_reset &= ~1UL;
    secure_app_deinit();
    configure_nonsecure_world();
#endif

    dwt_enable_cycle_counter();

#ifndef RUN_INSIDE_VM
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
#else
    crosshyp_hypercall(CROSSCON_HC_BOOT, 0, 0);
#endif

    while(1);
}

#else

/* RZ/T2M has no Cortex-M SAU/non-secure vector-table transition.  In the VM,
 * retain the existing hypercall path; a native non-secure payload needs a
 * platform-specific world-switch implementation. */
static void jump_to_nonsecure(void)
{
#ifdef RUN_INSIDE_VM
    crosshyp_hypercall(CROSSCON_HC_BOOT, 0, 0);
#endif
    while (1) {
        wfi();
    }
}

#endif /* PLAT_RZT2M */


void uart_rx_handler(unsigned int id)
{
    (void)id;
    uart_clear_rxirq();
    printf("Secure App: UART RX Handler\n");
#ifdef RUN_INSIDE_VM
    crosshyp_hypercall(CROSSCON_HC_END_INT, 0, 0); //notify hypervisor that the secure interrupt has been handled
#endif

}

uint32_t prev_cycle = 0;

void timer_handler(unsigned int id)
{
    (void)id;
#ifdef USINGTIMER
    clear_timer_interrupt();
#endif
#ifdef SECURE_INTERRUPT_LATENCY
#ifdef USINGTIMER
    collect_timer_latency_sample();
#else
    uint32_t latency_ns = (uint32_t)(((uint64_t)secure_systick_latency_cycles *
                                      1000000000ULL) / TIMER_FREQ);

    // printf("Secure SysTick latency: %u cycles, %u dwt cycles, %u ns (%u CVR, sample %u)\n",
    //        secure_systick_latency_cycles,
    //        dwt_cycles_at_interrupt_entry,
    //        latency_ns,
    //        secure_systick_entry_cvr,
    //        secure_systick_sample_count);
    
    uint32_t dwt_latency_ns = (uint32_t)((uint64_t)((dwt_cycles_at_interrupt_entry%1250000) *
                                      1000000000ULL) / PLAT_DWT_FREQ);
    prev_cycle = dwt_cycles_at_interrupt_entry;
    printf("SysTick latency: %u cycles, %u ns\n",
        secure_systick_latency_cycles,
        latency_ns);
    printf("DWT latency: %u cycles, %u ns\n",
        dwt_cycles_at_interrupt_entry%1250000,
        dwt_latency_ns);
           
#endif
    #ifdef RUN_INSIDE_VM
        #ifdef EXECUTION_IN_NONSECURE_CONTEXT
            crosshyp_hypercall(CROSSCON_HC_END_INT, 0, 0); //notify hypervisor that the secure interrupt has been handled
        #endif
    #endif

#endif
    
}

void main(void)
{
    printf("#############################\n");
    printf("This is the secure Baremetal! \n");
    printf("#############################\n");

    irq_set_handler(UART_IRQ_ID, uart_rx_handler);
    irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);
    irq_enable(UART_IRQ_ID);
    uart_enable_rxirq();

#ifdef SECURE_INTERRUPT_LATENCY
    irq_set_handler(TIMER_IRQ_ID, timer_handler);
    irq_set_prio(TIMER_IRQ_ID, IRQ_MAX_PRIO);
    irq_enable(TIMER_IRQ_ID);

#ifdef USINGTIMER
    timer_init(PLAT_TIMER_FREQ, 100000U); //100Hz
    clear_timer_interrupt();
#else
    //timer_set(TIMER_INTERVAL - 1);
    dwt_enable_cycle_counter();
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;
    timer_set(1250000-1); //100ms FREQ * desired Time = number of ticks
    
    //systick->cvr = 0;
#endif

#ifdef USINGTIMER
    timer_enable();
    asm volatile("cpsie i" ::: "memory");
#endif
#endif

#ifdef NONSECURE_INTERRUPT_LATENCY  //interrupt latency measurment in non-secure context
    //Jump to Non-secure
    jump_to_nonsecure();
#endif 

#ifdef EXECUTION_IN_SECURE_CONTEXT //interrupt latency measurment in secure context
    while(1);    
#else
    //Jump to Non-secure
    jump_to_nonsecure();
#endif

// This is the code that will run whenever non-secure make a call to secure
// We need that execution stops in Secure for interrupts to be triggered in non-secure
    while(1) wfi();
}
