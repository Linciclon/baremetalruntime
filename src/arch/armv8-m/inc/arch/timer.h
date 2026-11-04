/**
 * SPDX-License-Identifier: Apache-2.0U
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef ARCH_TIMER_H
#define ARCH_TIMER_H

#include <core.h>
#include <sysregs.h>

#ifdef USINGTIMER0
#include <lpc_timer0.h>
#else
#include <systick.h>
#endif

#define TIMER_FREQ  (PLAT_TIMER_FREQ)

static inline void timer_enable(void)
{
#ifdef USINGTIMER0
    ctimer0_start();
#else
    systick_set_csr( systick_get_csr() | SYSTICK_CSR_ENABLE | SYSTICK_CSR_CLKSOURCE);
#endif
}

static inline void timer_disable(void)
{
#ifdef USINGTIMER0
    ctimer0_stop();
#else
    systick_set_csr(systick_get_csr() & ~SYSTICK_CSR_ENABLE);
#endif
}

static inline void timer_set(uint64_t value)
{
#ifdef USINGTIMER0
    uint32_t ticks = value >= UINT32_MAX ? UINT32_MAX : (uint32_t)value + 1U;
    ctimer0_set_match_ticks(ticks);
#else
    timer_disable();
    systick_set_rvr(value);
    timer_enable();
#endif
}

static inline uint64_t timer_get(void)
{
#ifdef USINGTIMER0
    return (uint64_t)ctimer0_get();
#else
    return (uint64_t)systick_get_cvr();
#endif
}


#endif
