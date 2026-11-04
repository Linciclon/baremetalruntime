/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <lpc_timer0.h>
#include <nvic.h>
#include <syscon.h>

static ctimer0_cb_t ctimer0_cb;
static void* ctimer0_cb_arg;

static uint32_t ctimer0_calc_ticks_hz(uint32_t input_clk_hz, uint32_t period_hz)
{
    if (period_hz == 0U) {
        return 0U;
    }

    uint32_t ticks = input_clk_hz / period_hz;
    return ticks == 0U ? 1U : ticks;
}

static uint32_t ctimer0_calc_ticks_us(uint32_t input_clk_hz, uint32_t period_us)
{
    if (period_us == 0U) {
        return 0U;
    }

    uint64_t ticks = ((uint64_t)input_clk_hz * (uint64_t)period_us) / 1000000ULL;
    if (ticks == 0ULL) {
        return 1U;
    }
    if (ticks > UINT32_MAX) {
        return UINT32_MAX;
    }

    return (uint32_t)ticks;
}

void ctimer0_init_periodic_ticks(uint32_t ticks, ctimer0_cb_t cb, void* arg)
{
    if (ticks == 0U) {
        return;
    }

    ctimer0_cb = cb;
    ctimer0_cb_arg = arg;

    syscon_ctimer0_clk_enable();
    syscon_ctimer0_clk_src(SYSCON_CTIMERCLKSEL_MAINCLK);//SYSCON_CTIMERCLKSEL_FROHFDIV);
    syscon_ctimer0_reset();

    CTIMER0->tcr = CTIMER_TCR_RESET;
    CTIMER0->ctcr = CTIMER_CTCR_TIMER;
    CTIMER0->pr = 0U;
    CTIMER0->pc = 0U;
    CTIMER0->tc = 0U;
    CTIMER0->mr[0] = ticks - 1U;
    CTIMER0->mcr = CTIMER_MCR_MR0I | CTIMER_MCR_MR0R;
    CTIMER0->ir = CTIMER_IR_MR0INT;
    CTIMER0->tcr = 0U;

    nvic_set_pending(CTIMER0_IRQ_ID - EXT_INT_BASE, false);
}

void ctimer0_init_periodic_hz(uint32_t input_clk_hz, uint32_t period_hz,
    ctimer0_cb_t cb, void* arg)
{
    ctimer0_init_periodic_ticks(ctimer0_calc_ticks_hz(input_clk_hz, period_hz), cb, arg);
}

void ctimer0_init_periodic_us(uint32_t input_clk_hz, uint32_t period_us,
    ctimer0_cb_t cb, void* arg)
{
    ctimer0_init_periodic_ticks(ctimer0_calc_ticks_us(input_clk_hz, period_us), cb, arg);
}

void ctimer0_set_match_ticks(uint32_t ticks)
{
    ctimer0_init_periodic_ticks(ticks, ctimer0_cb, ctimer0_cb_arg);
    ctimer0_start();
}

void ctimer0_start(void)
{
    ctimer0_clear_irq();
    CTIMER0->tcr = CTIMER_TCR_CEN;
}

void ctimer0_stop(void)
{
    CTIMER0->tcr = 0U;
}

uint32_t ctimer0_get(void)
{
    return CTIMER0->tc;
}

bool ctimer0_irq_pending(void)
{
    return (CTIMER0->ir & CTIMER_IR_MR0INT) != 0U;
}

void ctimer0_clear_irq(void)
{
    CTIMER0->ir = CTIMER_IR_MR0INT;
}

void ctimer0_irq_handler(void)
{
    if (ctimer0_irq_pending()) {
        ctimer0_clear_irq();
        if (ctimer0_cb != NULL) {
            ctimer0_cb(ctimer0_cb_arg);
        }
    }
}

void ctimer0_periodic_init(uint32_t timer_clk_hz, uint32_t period_hz)
{
    ctimer0_init_periodic_hz(timer_clk_hz, period_hz, NULL, NULL);
}

void start_timer(uint32_t timer_clk_hz, uint32_t period_hz)
{
    (void)timer_clk_hz;
    (void)period_hz;
    ctimer0_start();
}

void clear_timer_interrupt(void)
{
    ctimer0_clear_irq();
}
