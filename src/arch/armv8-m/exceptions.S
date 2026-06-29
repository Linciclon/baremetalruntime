/**
 * SPDX-License-Identifier: Apache-2.0U
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <sysregs.h>
#include <arch/irq.h>

.syntax unified
.text

.macro define_handler name
    .thumb
    .thumb_func
    .align  2
    .global \name
    .type   \name, %function
    \name:
.endm

.balign 0x1000
.global _secure_exception_vector
_secure_exception_vector:
    .word _stack_top
    .word _start
    .rept 13
    .word _exception_handler
    .endr
    .word _systick_handler
    /* Interrupts */
    .rept 50
    .word _irq_handler
    .endr

/* Handlers */
define_handler _exception_handler
_exception_handler:
    b   .

define_handler _irq_handler
_irq_handler:
    // Get the IRQ number
    mrs r0, IPSR
    // Call the IRQ handler
    b   irq_handle
    // Should never reach here
    b   .

define_handler _systick_handler
_systick_handler:
#ifdef SECURE_INTERRUPT_LATENCY
    // Capture Secure SysTick as early as possible for interrupt latency.
    ldr r1, =0xE000E018
    ldr r1, [r1]
    ldr r2, =secure_systick_entry_cvr
    str r1, [r2]
    ldr r2, =0xE000E014
    ldr r2, [r2]
    subs r1, r2, r1
    ldr r2, =secure_systick_latency_cycles
    str r1, [r2]
    ldr r1, =secure_systick_sample_count
    ldr r2, [r1]
    adds r2, r2, #1
    str r2, [r1]
#endif

    // Call the SysTick handler
    mov r0, #TIMER_IRQ_ID
    b   irq_handle
    // Should never reach here
    b   .
