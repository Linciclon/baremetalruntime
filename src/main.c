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
#include <stdint.h>
#include <cpu.h>
#include <wfi.h>
#include <spinlock.h>
#include <plat.h>
#include <irq.h>
#include <uart.h>
#include <timer.h>
#include "crosshyp.h"
#if defined(PLAT_LPC55S69) && defined(USINGTIMER)
#include <lpc_timer0.h>
#endif

#define DWT_CTRL                 (*(volatile uint32_t *)0xE0001000UL)
#define DWT_CYCCNT               (*(volatile uint32_t *)0xE0001004UL)
#define DEMCR                    (*(volatile uint32_t *)0xE000EDFCUL)
#define DEMCR_TRCENA             (1UL << 24UL)
#define DWT_CTRL_CYCCNTENA       (1UL << 0UL)

#define PREFIX_MASK                           0xff000000
#define PREFIX                                0
#define CROSSCON_HC_END_INT                (PREFIX | 0xE)

#define TIMER_INTERVAL (TIME_S(1))

#define NUM_TICKS_FOR_BAD_ACCESS 20
#define BAD_ADDR (int *) 0x20031000

#ifdef PLAT_LPC55S69
void (*crosshyp_hypercall)(unsigned int, unsigned int, unsigned int) =
    (void (*)(unsigned int, unsigned int, unsigned int))CROSSHYP_HC_ADDR;
#endif

uint8_t Tick_Counter = 0;

volatile uint32_t dwt_s_to_ns_cycles __attribute__((section(".latency"), used));
volatile uint32_t dwt_ns_to_s_cycles;

extern uint32_t measure_ns_to_s_switch(void);

extern uint32_t jump_to_secure(void);

#ifndef PLAT_RZT2M
static uint32_t cycles_to_ns(uint32_t cycles)
{
    return (uint32_t)(((uint64_t)cycles * 1000000000ULL) / PLAT_DWT_FREQ);
}
#endif

#ifdef NONSECURE_INTERRUPT_LATENCY
volatile uint32_t nonsecure_systick_entry_cvr;
volatile uint32_t nonsecure_systick_latency_cycles;
volatile uint32_t nonsecure_systick_sample_count;
volatile uint32_t nonsecure_dwt_cycles_at_interrupt_entry;
#endif

#if defined(NONSECURE_INTERRUPT_LATENCY) && defined(USINGTIMER)
#define N_WARMUP_SAMPLES 500U
#define N_SAMPLES        100U

spinlock_t print_lock = SPINLOCK_INITVAL;
unsigned long samples[N_SAMPLES];

static void collect_timer_latency_sample(void)
{
    static unsigned long warmup_cnt = N_WARMUP_SAMPLES;
    static unsigned long sample_cnt;

    if (warmup_cnt != 0U) {
        warmup_cnt--;
        return;
    }
    if (sample_cnt >= N_SAMPLES) {
        return;
    }

    samples[sample_cnt++] = (unsigned long)nonsecure_systick_latency_cycles;
    if (sample_cnt == N_SAMPLES) {
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

void uart_rx_handler(unsigned int id)
{
    (void)id;
    uart_clear_rxirq();
    printf("Non-Secure App: UART RX Handler\n");
    #ifdef PLAT_LPC55S69 
    #ifdef RUN_INSIDE_VM
        crosshyp_hypercall(CROSSCON_HC_END_INT, 0, 0); //notify hypervisor that the secure interrupt has been handled
    #endif
    #endif
}

// void timer_handler()
// {
//     // printf("VM0: SysTick Handler\n");
//     // int *ptr_addr = BAD_ADDR;
//     // Tick_Counter++;
//     // if(Tick_Counter == NUM_TICKS_FOR_BAD_ACCESS)
//     // {
//     //     printf("Memory Access ADDR:%d\n", &ptr_addr);
//     //     /*Bad Memory Access here*/
//     //     *ptr_addr = 0xdead; 
//     // }
//     #ifdef NONSECURE_INTERRUPT_LATENCY
//         uint32_t latency_ns = (uint32_t)(((uint64_t)nonsecure_systick_latency_cycles *
//                                           1000000000ULL) / TIMER_FREQ);

//         printf("Non-Secure SysTick latency: %u cycles, %u ns (%u CVR, sample %u)\n",
//                nonsecure_systick_latency_cycles,
//                latency_ns,
//                nonsecure_systick_entry_cvr,
//                nonsecure_systick_sample_count);
//     #endif
// }


uint32_t prev_cycle = 0;

void timer_handler(unsigned int id)
{
    (void)id;
#ifdef NONSECURE_INTERRUPT_LATENCY
    #ifdef USINGTIMER
        clear_timer_interrupt();
        collect_timer_latency_sample();
    #else
    uint32_t latency_ns = (uint32_t)(((uint64_t)nonsecure_systick_latency_cycles *
                                      1000000000ULL) / TIMER_FREQ);
    // printf("Non-Secure SysTick latency: %u cycles, %u dwt cycles, %u ns (%u CVR, sample %u)\n",
    //        nonsecure_systick_latency_cycles,
    //        nonsecure_dwt_cycles_at_interrupt_entry,
    //        latency_ns,
    //        nonsecure_systick_entry_cvr,
    //        nonsecure_systick_sample_count);
    
    uint32_t dwt_latency_ns = (uint32_t)((uint64_t)((nonsecure_dwt_cycles_at_interrupt_entry%1250000) *
                                      1000000000ULL) / PLAT_DWT_FREQ);
    prev_cycle = nonsecure_dwt_cycles_at_interrupt_entry;
    printf("SysTick latency: %u cycles, %u ns\n",
        nonsecure_systick_latency_cycles,
        latency_ns);
    printf("DWT latency: %u cycles, %u ns\n",
        nonsecure_dwt_cycles_at_interrupt_entry%1250000,
        dwt_latency_ns);
           
    #endif /* USINGTIMER */

#ifdef PLAT_LPC55S69 
    //it need to sinalise end of interrupt in armv8-m
    #ifdef RUN_INSIDE_VM
        #ifdef EXECUTION_IN_SECURE_CONTEXT
            crosshyp_hypercall(CROSSCON_HC_END_INT, 0, 0); //notify hypervisor that the secure interrupt has been handled
        #endif
    #endif
#endif
#endif
    
}




#ifndef PLAT_RZT2M
volatile uint32_t dbg_ipsr;
volatile uint32_t dbg_primask;
volatile uint32_t dbg_basepri;
volatile uint32_t dbg_faultmask;
volatile uint32_t dbg_control;

static inline uint32_t read_ipsr(void)
{
    uint32_t v;
    __asm volatile ("mrs %0, ipsr" : "=r" (v));
    return v;
}

static inline uint32_t read_primask(void)
{
    uint32_t v;
    __asm volatile ("mrs %0, primask" : "=r" (v));
    return v;
}

static inline uint32_t read_basepri(void)
{
    uint32_t v;
    __asm volatile ("mrs %0, basepri" : "=r" (v));
    return v;
}

static inline uint32_t read_faultmask(void)
{
    uint32_t v;
    __asm volatile ("mrs %0, faultmask" : "=r" (v));
    return v;
}

static inline uint32_t read_control(void)
{
    uint32_t v;
    __asm volatile ("mrs %0, control" : "=r" (v));
    return v;
}

void debug_read_exception_state(void)
{
    dbg_ipsr      = read_ipsr();
    dbg_primask   = read_primask();
    dbg_basepri   = read_basepri();
    dbg_faultmask = read_faultmask();
    dbg_control   = read_control();

    __asm volatile ("bkpt #0");
}

static void dwt_enable_cycle_counter(void)
{
    DEMCR |= DEMCR_TRCENA;
    DWT_CYCCNT = 0;
    //DWT_CTRL |= DWT_CTRL_CYCCNTENA;
    //asm volatile("dsb\nisb" ::: "memory");
}
#endif /* PLAT_RZT2M */

void main(void)
{
    printf("#############################\n");
    printf("This is the Non-secure Baremetal! \n");
    printf("#############################\n");

#ifndef PLAT_RZT2M
    printf("DWT S->NS switch: %u cycles, %u ns\n",
           dwt_s_to_ns_cycles, cycles_to_ns(dwt_s_to_ns_cycles));
#endif

    irq_set_handler(UART_IRQ_ID, uart_rx_handler);

    uart_enable_rxirq();

    irq_enable(UART_IRQ_ID);
    irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);

#ifdef CONTEXT_SWITCH_LAT
    dwt_ns_to_s_cycles = measure_ns_to_s_switch();
    printf("DWT NS->S gateway switch: %u cycles, %u ns\n",
           dwt_ns_to_s_cycles, cycles_to_ns(dwt_ns_to_s_cycles));
    dwt_ns_to_s_cycles = measure_ns_to_s_switch();
    printf("DWT NS->S gateway switch: %u cycles, %u ns\n",
           dwt_ns_to_s_cycles, cycles_to_ns(dwt_ns_to_s_cycles));
    dwt_ns_to_s_cycles = measure_ns_to_s_switch();
    printf("DWT NS->S gateway switch: %u cycles, %u ns\n",
           dwt_ns_to_s_cycles, cycles_to_ns(dwt_ns_to_s_cycles));
#endif

    // Tick_Counter = 0; 

#ifdef NONSECURE_INTERRUPT_LATENCY
    irq_set_handler(TIMER_IRQ_ID, timer_handler);
    irq_enable(TIMER_IRQ_ID);
    irq_set_prio(TIMER_IRQ_ID, IRQ_MAX_PRIO);

    #ifdef USINGTIMER
        timer_init(PLAT_TIMER_FREQ, 10000U);
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

#ifdef EXECUTION_IN_NONSECURE_CONTEXT
while(1);//acerescentei isto para validar as interrupções do timer
#endif

#ifdef EXECUTION_IN_SECURE_CONTEXT
#ifndef RUN_INSIDE_VM
#ifdef PLAT_RZT2M
    /* A native Cortex-R52 image has no Cortex-M NSC gateway.  Keep executing
     * locally; an EL2/secure monitor is required for a real world switch. */
    while (1) {
        wfi();
    }
#else
//Jump to a secure context to measure secure interrupt latency
    jump_to_secure();
#endif
#else
    //Jump to a secure context to measure secure interrupt latency
    crosshyp_hypercall(0, 0, 0); //invoke service from VM parent (0), hypercall id 0, no args
    while(1) {
        crosshyp_hypercall(CROSSCON_HC_END_INT, 0, 0); //notify hypervisor that the secure interrupt has been handled
    }
#endif
#endif


#ifdef CONTEXT_SWITCH_LAT
    //Jump to secure to measure non-secure interrupt latency 
    measure_ns_to_s_switch();
#endif

    // debug_read_exception_state();

    while(1);
}
