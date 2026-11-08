#ifndef TIMER_H_
#define TIMER_H_

#include <core.h>
#include <arch/timer.h>

#define TIME_US(us) (((TIMER_FREQ)*(us))/(1000000ull))
#define TIME_MS(ms) (TIME_US((ms)*1000ull))
#define TIME_S(s)   (TIME_MS((s)*1000ull))

uint64_t timer_get();
void timer_set(uint64_t n);

#ifdef USINGTIMER
void timer_init(uint32_t timer_clk_hz, uint32_t period_hz);
void clear_timer_interrupt(void);
#endif

static inline void timer_wait(uint64_t n) {
    uint64_t start = timer_get();
    while(timer_get() < (start+n));
}

#endif
