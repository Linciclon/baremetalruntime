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

#define TIMER_INTERVAL (TIME_S(1))

#define NUM_TICKS_FOR_BAD_ACCESS 20
#define BAD_ADDR (int *) 0x20031000

uint8_t Tick_Counter = 0;

void uart_rx_handler()
{
    uart_clear_rxirq();
    printf("Non-Secure App: UART RX Handler\n");
}

void timer_handler()
{
    printf("VM0: SysTick Handler\n");
    // int *ptr_addr = BAD_ADDR;
    // Tick_Counter++;
    // if(Tick_Counter == NUM_TICKS_FOR_BAD_ACCESS)
    // {
    //     printf("Memory Access ADDR:%d\n", &ptr_addr);
    //     /*Bad Memory Access here*/
    //     *ptr_addr = 0xdead; 
    // }
}

#include <stdint.h>

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


void main(void)
{
    printf("Initializing Non-secure Bare-metal APP! \n");

    irq_set_handler(UART_IRQ_ID, uart_rx_handler);
    // irq_set_handler(TIMER_IRQ_ID, timer_handler);

    uart_enable_rxirq();

    // Tick_Counter = 0; 

    // timer_set(TIMER_INTERVAL);
    // irq_enable(TIMER_IRQ_ID);
    // irq_set_prio(TIMER_IRQ_ID, IRQ_MAX_PRIO);

    irq_enable(UART_IRQ_ID);
    irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);

    // debug_read_exception_state();

    while(1) wfi();
}
