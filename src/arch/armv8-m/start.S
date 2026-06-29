/**
 * SPDX-License-Identifier: Apache-2.0U
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <sysregs.h>

.syntax unified

.section .start, "ax"
.thumb
.thumb_func
.global _start
_start:
    // Capture DWT as soon as Non-secure execution starts.
    ldr r0, =0xE0001004
    ldr r0, [r0]
    ldr r1, =dwt_s_to_ns_cycles
    str r0, [r1]

    // Disable interrupts
    cpsid i

    // Config VTOR & MSPLIM register
    ldr r0, =SCB_BASE
    ldr r1, =_exception_vector
    str r1, [r0, #SCB_VTOR_OFF]
    // Read the MSP from vector table
    ldr r1, [r1]
    msr msp, r1
    ldr r3, =_stack_base
    msr msplim, r3

    // Enable Fault Handlers
    ldr r4, [r0, #SCB_SHCSR_OFF]
    orr r4, r4, #SCB_SHCSR_EN_ALL_FAULTS
    str r4, [r0, #SCB_SHCSR_OFF]

    // Copy data from RX to RWX
    ldr r7, =_data_lma_start        // LMA start
    ldr r11, =_data_vma_start       // VMA start
    ldr r12, =_data_lma_end         // LMA end
    bl copy_data

    // Clear bss
    ldr r11, =__bss_start
    ldr r12, =__bss_end
    bl boot_clear

    isb

    // TODO:ARMV8M - Enable floating point
    // TODO:ARMV8M - Configure MPU

/*
#ifdef MPU

    dsb
    isb

#endif
*/

    // Reenable interrupts
    cpsie i

    // TODO: other c runtime init (eg ctors)
    // TODO:ARMV8M - If MPU is configured, we need to switch to unprivileged mode ?

    b _init
    b _exit

.global boot_clear
boot_clear:
2:
    mov r8, #0
	cmp r11, r12
	bge 1f
	str r8, [r11]
	add r11, r11, #4
	b 2b
1:
	bx lr

/* Copies data from r7 to r11 up to the r12 limit */
.global copy_data
copy_data:
1:
    ldr r8, [r7], #4
    str r8, [r11], #4
    cmp r7, r12
    bne 1b
    bx lr
