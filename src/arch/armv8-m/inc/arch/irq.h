/**
 * SPDX-License-Identifier: Apache-2.0U
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include <plat.h>

#ifdef USINGTIMER0
#include <lpc_timer0.h>
#endif

#define IPI_IRQ_ID      (0)
#ifdef USINGTIMER0
#define TIMER_IRQ_ID    (CTIMER0_IRQ_ID)//use ctimer0 timer
#else
#define TIMER_IRQ_ID    (15)            //use systick timer
#endif
#define IRQ_NUM         (PLAT_MAX_INTERRUPTS)
#define IRQ_MAX_PRIO    (0)

#endif /* ARCH_IRQ_H */
