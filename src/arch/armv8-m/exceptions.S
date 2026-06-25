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
.global _exception_vector
_exception_vector:
    .word _stack_top
    .word _start
    .rept 13
    .word _exception_handler
    .endr
    .word _systick_handler
    /* Interrupts */
    .rept IRQ_NUM
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
    // Call the SysTick handler
    mov r0, #TIMER_IRQ_ID
    b   irq_handle
    // Should never reach here
    b   .
