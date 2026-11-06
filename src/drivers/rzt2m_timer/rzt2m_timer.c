/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * Periodic timer support for RZ/T2M MTU channel 4 or 7.  This deliberately
 * uses the same overflow interrupt path as the known-good baremetal25 app.
 */

#include <rzt2m_timer.h>
#include <uart_api.h>

#ifndef MTU_CHANNEL
#define MTU_CHANNEL 4
#endif

#define RZT2M_MTU_COMMON_BASE       0x90001000UL
#define RZT2M_MTU4_BASE             0x90001200UL
#define RZT2M_MTU7_BASE             0x90001A00UL

#define RZT2M_MTU_TOERA             (*(volatile uint8_t *)(RZT2M_MTU_COMMON_BASE + 0x20AU))
#define RZT2M_MTU_TSTRA             (*(volatile uint8_t *)(RZT2M_MTU_COMMON_BASE + 0x280U))
#define RZT2M_MTU_TRWERA            (*(volatile uint8_t *)(RZT2M_MTU_COMMON_BASE + 0x284U))
#define RZT2M_MTU_TOERB             (*(volatile uint8_t *)(RZT2M_MTU_COMMON_BASE + 0xA0AU))
#define RZT2M_MTU_TSTRB             (*(volatile uint8_t *)(RZT2M_MTU_COMMON_BASE + 0xA80U))
#define RZT2M_MTU_TRWERB            (*(volatile uint8_t *)(RZT2M_MTU_COMMON_BASE + 0xA84U))

#define RZT2M_MTU_TSTRA_CST4        (1U << 7)
#define RZT2M_MTU_TSTRB_CST7        (1U << 7)
#define RZT2M_MTU_TRWER_RWE         (1U << 0)
#define RZT2M_MTU_TIER_TCIEV        (1U << 4)
#define RZT2M_MTU_TSR_TCFV          (1U << 4)
#define RZT2M_MTU_OUTPUT_ENABLES     0x3FU
#define RZT2M_MTU_CKEG_FALLING      (1U << 3)
#define RZT2M_MTU_COUNTER_COUNTS     (UINT16_MAX + 1U)

#if MTU_CHANNEL == 4
typedef R_MTU4 rzt2m_mtu_timer_t;
#define RZT2M_MTU_TIMER_BASE         RZT2M_MTU4_BASE
#elif MTU_CHANNEL == 7
typedef R_MTU7_Type rzt2m_mtu_timer_t;
#define RZT2M_MTU_TIMER_BASE         RZT2M_MTU7_BASE
#else
#error "Unsupported MTU_CHANNEL; supported channels are 4 and 7"
#endif

struct rzt2m_mtu_divider {
    uint16_t divisor;
    uint8_t encoding;
};

/* Low three encoding bits select TCR.TPSC.  The upper bits select
 * TCR2.TPSC2, following the Renesas FSP divider table for channels 4/7. */
static const struct rzt2m_mtu_divider rzt2m_mtu_dividers[] = {
    {1U, 0U}, {2U, 8U}, {4U, 1U}, {8U, 16U}, {16U, 2U},
    {32U, 24U}, {64U, 3U}, {256U, 4U}, {1024U, 5U},
};

extern unsigned long TIMER_FREQ;

static volatile rzt2m_mtu_timer_t * const timer =
    (volatile rzt2m_mtu_timer_t *)RZT2M_MTU_TIMER_BASE;
static uint16_t rzt2m_timer_reload;

static inline void rzt2m_timer_barrier(void)
{
    __asm__ volatile("dsb sy" ::: "memory");
}

void rzt2m_timer_disable(void)
{
#if MTU_CHANNEL == 4
    RZT2M_MTU_TSTRA &= (uint8_t)~RZT2M_MTU_TSTRA_CST4;
#else
    RZT2M_MTU_TSTRB &= (uint8_t)~RZT2M_MTU_TSTRB_CST7;
#endif
    rzt2m_timer_barrier();
}

void clear_timer_interrupt(void)
{
    timer->TSR &= (uint8_t)~RZT2M_MTU_TSR_TCFV;
    timer->TCNT = rzt2m_timer_reload;
    rzt2m_timer_barrier();
}

void timer_init(uint32_t timer_clk_hz, uint32_t period_hz)
{
    uint32_t counts = 0U;
    uint8_t divider_encoding = 0U;
    uint16_t divider = 1U;

    register_protect_disable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_MTU3, MTU_CHANNEL);
    register_protect_enable(BSP_REG_PROTECT_LPC_RESET);

    rzt2m_timer_disable();

    if ((timer_clk_hz == 0U) || (period_hz == 0U)) {
        timer->TIER = 0U;
        return;
    }

    for (size_t i = 0U;
         i < (sizeof(rzt2m_mtu_dividers) / sizeof(rzt2m_mtu_dividers[0]));
         i++) {
        divider = rzt2m_mtu_dividers[i].divisor;
        counts = (uint32_t)((uint64_t)timer_clk_hz /
                            ((uint64_t)divider * period_hz));
        if ((counts > 0U) && (counts <= RZT2M_MTU_COUNTER_COUNTS)) {
            divider_encoding = rzt2m_mtu_dividers[i].encoding;
            break;
        }
    }

    if (counts == 0U) {
        counts = 1U;
    } else if (counts > RZT2M_MTU_COUNTER_COUNTS) {
        counts = RZT2M_MTU_COUNTER_COUNTS;
    }

    TIMER_FREQ = timer_clk_hz / divider;
    rzt2m_timer_reload = (uint16_t)(RZT2M_MTU_COUNTER_COUNTS - counts);

    /* Match the register setup used by baremetal25. */
    RZT2M_MTU_TOERA = RZT2M_MTU_OUTPUT_ENABLES;
    RZT2M_MTU_TOERB = RZT2M_MTU_OUTPUT_ENABLES;
#if MTU_CHANNEL == 4
    RZT2M_MTU_TRWERA |= RZT2M_MTU_TRWER_RWE;
#else
    RZT2M_MTU_TRWERB |= RZT2M_MTU_TRWER_RWE;
#endif

    timer->TIER = 0U;
    timer->TMDR1 = 0U;
    timer->TCR = (uint8_t)(RZT2M_MTU_CKEG_FALLING |
                            (divider_encoding & 0x7U));
    timer->TCR2 = (uint8_t)((divider_encoding >> 3U) & 0x7U);
    timer->TGRA = 0U;
    clear_timer_interrupt();
    timer->TIER = RZT2M_MTU_TIER_TCIEV;
    rzt2m_timer_barrier();
}

void rzt2m_timer_enable(void)
{
    clear_timer_interrupt();
#if MTU_CHANNEL == 4
    RZT2M_MTU_TSTRA |= RZT2M_MTU_TSTRA_CST4;
#else
    RZT2M_MTU_TSTRB |= RZT2M_MTU_TSTRB_CST7;
#endif
    rzt2m_timer_barrier();
}

void rzt2m_timer_set(uint64_t value)
{
    uint32_t counts = value >= UINT16_MAX ?
                      RZT2M_MTU_COUNTER_COUNTS : (uint32_t)value + 1U;

    rzt2m_timer_disable();
    rzt2m_timer_reload = (uint16_t)(RZT2M_MTU_COUNTER_COUNTS - counts);
    clear_timer_interrupt();
    rzt2m_timer_enable();
}

uint64_t rzt2m_timer_get(void)
{
    return (uint64_t)timer->TCNT;
}
