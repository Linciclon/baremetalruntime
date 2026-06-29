/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <plat.h>

.syntax unified
.text
.thumb

.thumb_func
.align 2
.global measure_ns_to_s_switch
.type measure_ns_to_s_switch, %function
measure_ns_to_s_switch:
    ldr r2, =0xE0001004
    movs r1, #0
    //str r1, [r2]
    ldr r0, =(PLAT_NSC_DWT_PROBE_ADDR | 1)
    str r1, [r2]
    bx r0

.size measure_ns_to_s_switch, . - measure_ns_to_s_switch
