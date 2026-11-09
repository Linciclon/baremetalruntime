/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <plat.h>

.syntax unified
.text
.thumb

.equ CROSSHYP_HC_ADDR, 0x10000041

#if defined(CONTEXT_SWITCH_LATENCY) && defined(RUN_INSIDE_VM)
.thumb_func
.align 2
.global measure_vm_context_switch
.type measure_vm_context_switch, %function
measure_vm_context_switch:
    push {r4, lr}
    mov r4, r0

    ldr r2, =0xE0001004
    movs r1, #0
    str r1, [r2]
    ldr r2, =0xE0001000
    ldr r1, [r2]
    orr r1, r1, #1
    str r1, [r2]
    dsb
    isb

    mov r0, r4
    movs r1, #0
    movs r2, #0
    ldr r3, =CROSSHYP_HC_ADDR
    blx r3

    /* First guest instruction after the hypervisor resumes this VM. */
    ldr r1, =0xE0001004
    ldr r0, [r1]
    ldr r1, =0xE0001000
    ldr r2, [r1]
    bic r2, r2, #1
    str r2, [r1]
    dsb
    isb
    pop {r4, pc}

.size measure_vm_context_switch, . - measure_vm_context_switch
#endif

#ifndef RUN_INSIDE_VM
.thumb_func
.align 2
.global measure_ns_to_s_switch
.type measure_ns_to_s_switch, %function
measure_ns_to_s_switch:
    push {r4, lr}
    ldr r2, =0xE0001004
    movs r1, #0
    str r1, [r2]
    ldr r2, =0xE0001000
    ldr r1, [r2]
    orr r1, r1, #1
    str r1, [r2]
    dsb
    isb

    /* Make BXNS LR return here, so S->NS is captured immediately. */
    adr r1, 1f
    orr r1, r1, #1
    mov lr, r1
    ldr r0, =(PLAT_NSC_DWT_PROBE_ADDR | 1)
    bx r0
1:
    ldr r1, =0xE0001004
    ldr r0, [r1]
    ldr r1, =0xE0001000
    ldr r2, [r1]
    bic r2, r2, #1
    str r2, [r1]
    dsb
    isb
    pop {r4, pc}

.size measure_ns_to_s_switch, . - measure_ns_to_s_switch

.thumb_func
.align 2
.global jump_to_secure
.type jump_to_secure, %function
jump_to_secure:
    b measure_ns_to_s_switch
#endif
