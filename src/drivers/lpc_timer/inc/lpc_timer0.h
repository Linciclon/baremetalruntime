/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef LPC_TIMER0_H
#define LPC_TIMER0_H

#include <core.h>
#include <sysregs.h>

#define CTIMER0_BASE_NS 0x40008000UL
#define CTIMER0_BASE_S  0x50008000UL

#ifndef RUN_INSIDE_VM
#ifdef SECURE_INTERRUPT_LATENCY
#define CTIMER0_BASE CTIMER0_BASE_S
#endif
#if defined(NONSECURE_INTERRUPT_LATENCY)
#define CTIMER0_BASE CTIMER0_BASE_NS
#endif
#else
#define CTIMER0_BASE CTIMER0_BASE_NS
#endif

#ifndef CTIMER0_PERIPH_IRQ_ID
#define CTIMER0_PERIPH_IRQ_ID 10
#endif

#ifndef CTIMER0_IRQ_ID
#ifdef __ASSEMBLER__
#define CTIMER0_IRQ_ID 26
#else
#define CTIMER0_IRQ_ID (CTIMER0_PERIPH_IRQ_ID + EXT_INT_BASE)
#endif
#endif

#define CTIMER_IR_MR0INT  (1UL << 0)

#define CTIMER_TCR_CEN    (1UL << 0)
#define CTIMER_TCR_RESET  (1UL << 1)

#define CTIMER_MCR_MR0I   (1UL << 0)
#define CTIMER_MCR_MR0R   (1UL << 1)
#define CTIMER_MCR_MR0S   (1UL << 2)

#define CTIMER_CTCR_TIMER (0UL << 0)

#ifndef __ASSEMBLER__

/** CTIMER - Register Layout Typedef */
typedef struct {
  volatile uint32_t ir;                                /**< Interrupt Register. The IR can be written to clear interrupts. The IR can be read to identify which of eight possible interrupt sources are pending., offset: 0x0 */
  volatile uint32_t tcr;                               /**< Timer Control Register. The TCR is used to control the Timer Counter functions. The Timer Counter can be disabled or reset through the TCR., offset: 0x4 */
  volatile uint32_t tc;                                /**< Timer Counter, offset: 0x8 */
  volatile uint32_t pr;                                /**< Prescale Register, offset: 0xC */
  volatile uint32_t pc;                                /**< Prescale Counter, offset: 0x10 */
  volatile uint32_t mcr;                               /**< Match Control Register, offset: 0x14 */
  volatile uint32_t mr[4];                             /**< Match Register . MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC., array offset: 0x18, array step: 0x4 */
  volatile uint32_t ccr;                               /**< Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place., offset: 0x28 */
  volatile const  uint32_t cr[4];                             /**< Capture Register . CR is loaded with the value of TC when there is an event on the CAPn. input., array offset: 0x2C, array step: 0x4 */
  volatile uint32_t emr;                               /**< External Match Register. The EMR controls the match function and the external match pins., offset: 0x3C */
       uint8_t RESERVED_0[48];
  volatile uint32_t ctcr;                              /**< Count Control Register. The CTCR selects between Timer and Counter mode, and in Counter mode selects the signal and edge(s) for counting., offset: 0x70 */
  volatile uint32_t pwmc;                              /**< PWM Control Register. This register enables PWM mode for the external match pins., offset: 0x74 */
  volatile uint32_t msr[4];                            /**< Match Shadow Register, array offset: 0x78, array step: 0x4 */
} ctimer_t;

typedef void (*ctimer0_cb_t)(void* arg);

#define CTIMER0 ((ctimer_t*)CTIMER0_BASE)

void ctimer0_init_periodic_ticks(uint32_t ticks, ctimer0_cb_t cb, void* arg);
void ctimer0_init_periodic_hz(uint32_t input_clk_hz, uint32_t period_hz,
    ctimer0_cb_t cb, void* arg);
void ctimer0_init_periodic_us(uint32_t input_clk_hz, uint32_t period_us,
    ctimer0_cb_t cb, void* arg);
void ctimer0_set_match_ticks(uint32_t ticks);
void ctimer0_start(void);
void ctimer0_stop(void);
uint32_t ctimer0_get(void);
bool ctimer0_irq_pending(void);
void ctimer0_clear_irq(void);
void ctimer0_irq_handler(void);

void ctimer0_periodic_init(uint32_t timer_clk_hz, uint32_t period_hz);
void start_timer(uint32_t timer_clk_hz, uint32_t period_hz);
void clear_timer_interrupt(void);
void timer_init(uint32_t timer_clk_hz, uint32_t period_hz);

#endif

#endif /* LPC_TIMER0_H */
