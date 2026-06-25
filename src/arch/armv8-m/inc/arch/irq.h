/**
 * SPDX-License-Identifier: Apache-2.0U
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include <plat.h>

#define IPI_IRQ_ID      (0)
#define TIMER_IRQ_ID    (15)
#define IRQ_NUM         (PLAT_MAX_INTERRUPTS+16)
#define IRQ_MAX_PRIO    (0)

#endif /* ARCH_IRQ_H */
