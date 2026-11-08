/** SPDX-License-Identifier: Apache-2.0 */
#include <rzt2m_timer.h>
#include <uart_api.h>

#ifndef MTU_CHANNEL
#define MTU_CHANNEL 4
#endif

#define MTU_COMMON_BASE       0x90001000UL
#define MTU4_BASE             0x90001200UL
#define MTU7_BASE             0x90001A00UL
#define MTU_TOERA             (*(volatile uint8_t *)(MTU_COMMON_BASE + 0x20AU))
#define MTU_TSTRA             (*(volatile uint8_t *)(MTU_COMMON_BASE + 0x280U))
#define MTU_TRWERA            (*(volatile uint8_t *)(MTU_COMMON_BASE + 0x284U))
#define MTU_TOERB             (*(volatile uint8_t *)(MTU_COMMON_BASE + 0xA0AU))
#define MTU_TSTRB             (*(volatile uint8_t *)(MTU_COMMON_BASE + 0xA80U))
#define MTU_TRWERB            (*(volatile uint8_t *)(MTU_COMMON_BASE + 0xA84U))
#define MTU_START             (1U << 7)
#define MTU_RWE               (1U << 0)
#define MTU_TCIEV             (1U << 4)
#define MTU_TCFV              (1U << 4)
#define MTU_CKEG_FALLING      (1U << 3)
#define MTU_COUNTER_COUNTS    (UINT16_MAX + 1U)

#if MTU_CHANNEL == 4
typedef R_MTU4 mtu_timer_t;
#define MTU_TIMER_BASE MTU4_BASE
#elif MTU_CHANNEL == 7
typedef R_MTU7_Type mtu_timer_t;
#define MTU_TIMER_BASE MTU7_BASE
#else
#error "Unsupported MTU_CHANNEL; supported channels are 4 and 7"
#endif

struct mtu_divider { uint16_t divisor; uint8_t encoding; };
static const struct mtu_divider dividers[] = {
    {1U, 0U}, {2U, 8U}, {4U, 1U}, {8U, 16U}, {16U, 2U},
    {32U, 24U}, {64U, 3U}, {256U, 4U}, {1024U, 5U},
};

extern unsigned long TIMER_FREQ;
static volatile mtu_timer_t * const timer =
    (volatile mtu_timer_t *)MTU_TIMER_BASE;
static uint16_t timer_reload;

static inline void timer_barrier(void)
{
    __asm__ volatile("dsb sy" ::: "memory");
}

void rzt2m_timer_disable(void)
{
#if MTU_CHANNEL == 4
    MTU_TSTRA &= (uint8_t)~MTU_START;
#else
    MTU_TSTRB &= (uint8_t)~MTU_START;
#endif
    timer_barrier();
}

void clear_timer_interrupt(void)
{
    timer->TSR &= (uint8_t)~MTU_TCFV;
    timer->TCNT = timer_reload;
    timer_barrier();
}

void timer_init(uint32_t timer_clk_hz, uint32_t period_hz)
{
    uint32_t counts = 0U;
    uint8_t encoding = 0U;
    uint16_t divisor = 1U;

    register_protect_disable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_MTU3, MTU_CHANNEL);
    register_protect_enable(BSP_REG_PROTECT_LPC_RESET);
    rzt2m_timer_disable();

    if ((timer_clk_hz == 0U) || (period_hz == 0U)) {
        timer->TIER = 0U;
        return;
    }

    for (size_t i = 0U; i < sizeof(dividers) / sizeof(dividers[0]); i++) {
        divisor = dividers[i].divisor;
        counts = (uint32_t)((uint64_t)timer_clk_hz /
                            ((uint64_t)divisor * period_hz));
        if ((counts > 0U) && (counts <= MTU_COUNTER_COUNTS)) {
            encoding = dividers[i].encoding;
            break;
        }
    }
    if (counts == 0U) counts = 1U;
    else if (counts > MTU_COUNTER_COUNTS) counts = MTU_COUNTER_COUNTS;

    TIMER_FREQ = timer_clk_hz / divisor;
    timer_reload = (uint16_t)(MTU_COUNTER_COUNTS - counts);
    MTU_TOERA = 0x3FU;
    MTU_TOERB = 0x3FU;
#if MTU_CHANNEL == 4
    MTU_TRWERA |= MTU_RWE;
#else
    MTU_TRWERB |= MTU_RWE;
#endif
    timer->TIER = 0U;
    timer->TMDR1 = 0U;
    timer->TCR = (uint8_t)(MTU_CKEG_FALLING | (encoding & 0x7U));
    timer->TCR2 = (uint8_t)((encoding >> 3U) & 0x7U);
    timer->TGRA = 0U;
    clear_timer_interrupt();
    timer->TIER = MTU_TCIEV;
    timer_barrier();
}

void rzt2m_timer_enable(void)
{
    clear_timer_interrupt();
#if MTU_CHANNEL == 4
    MTU_TSTRA |= MTU_START;
#else
    MTU_TSTRB |= MTU_START;
#endif
    timer_barrier();
}

void rzt2m_timer_set(uint64_t value)
{
    uint32_t counts = value >= UINT16_MAX ?
                      MTU_COUNTER_COUNTS : (uint32_t)value + 1U;
    rzt2m_timer_disable();
    timer_reload = (uint16_t)(MTU_COUNTER_COUNTS - counts);
    clear_timer_interrupt();
    rzt2m_timer_enable();
}

uint64_t rzt2m_timer_get(void)
{
    return (uint64_t)timer->TCNT;
}
