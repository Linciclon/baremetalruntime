/**
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RZT2M_TIMER_H
#define RZT2M_TIMER_H

#include <core.h>

void rzt2m_timer_enable(void);
void rzt2m_timer_disable(void);
void rzt2m_timer_set(uint64_t value);
uint64_t rzt2m_timer_get(void);

#endif /* RZT2M_TIMER_H */
