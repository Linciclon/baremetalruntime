#ifndef ARCH_TIMER_H
#define ARCH_TIMER_H

#include <core.h>
#include <sysregs.h>

extern unsigned long TIMER_FREQ;

#ifdef USINGTIMER

#include <rzt2m_timer.h>

static inline void timer_enable(void)
{
    rzt2m_timer_enable();
}

static inline void timer_disable(void)
{
    rzt2m_timer_disable();
}

static inline void timer_set(uint64_t value)
{
    rzt2m_timer_set(value);
}

static inline uint64_t timer_get(void)
{
    return rzt2m_timer_get();
}

#else

static inline void timer_set(uint64_t n)
{
    uint64_t current = sysreg_cntvct_el0_read();
    sysreg_cntv_cval_el0_write(current + n);
}

static inline uint64_t timer_get()
{
    uint64_t time = sysreg_cntvct_el0_read();
    return time; // assumes plat_freq = 100MHz
}

#endif

#endif
