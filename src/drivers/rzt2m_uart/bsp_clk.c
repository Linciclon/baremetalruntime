#include "bsp_clk.h"
#include "renesas.h"

#include <stddef.h>
#include <stdint.h>



#define BSP_CLOCKS_CLMA0_ERROR_NOT_MASK                     (1)    // CLMA0 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA1_ERROR_MASK                         (0)    // CLMA1 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA1_ERROR_NOT_MASK                     (1)    // CLMA1 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA3_ERROR_MASK                         (0)    // CLMA3 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA3_ERROR_NOT_MASK                     (1)    // CLMA3 error is transferred to POE3 and POEG.

#define BSP_CFG_CLMA3MASK (BSP_CLOCKS_CLMA3_ERROR_NOT_MASK) /* CLMA3 error not mask */
#define BSP_CFG_CLMA1MASK (BSP_CLOCKS_CLMA1_ERROR_MASK) /* CLMA1 error mask */
#define BSP_CFG_CLMA3_ENABLE (BSP_CLOCKS_CLMA3_ENABLE) /* CLMA3 Enabled */
#define BSP_CFG_CLMA1_ENABLE (BSP_CLOCKS_CLMA1_ENABLE) /* CLMA1 Enabled */
#define BSP_CFG_CLMA2_ENABLE (BSP_CLOCKS_CLMA2_ENABLE) /* CLMA2 Enabled */

#define BSP_CFG_CLMA0_CMPH (1023) /* CLMA0 CMPH 1023 */
#define BSP_CFG_CLMA1_CMPH (1023) /* CLMA1 CMPH 1023 */
#define BSP_CFG_CLMA2_CMPH (1023) /* CLMA2 CMPH 1023 */
#define BSP_CFG_CLMA3_CMPH (1023) /* CLMA3 CMPH 1023 */

#define BSP_CFG_CLMA0_CMPL (1) /* CLMA0 CMPL 1 */
#define BSP_CFG_CLMA1_CMPL (1) /* CLMA1 CMPL 1 */
#define BSP_CFG_CLMA2_CMPL (1) /* CLMA2 CMPL 1 */
#define BSP_CFG_CLMA3_CMPL (1) /* CLMA3 CMPL 1 */
#define BSP_CFG_CLMA0MASK (BSP_CLOCKS_CLMA0_ERROR_NOT_MASK) /* CLMA0 error not mask */
#define BSP_CLOCKS_CLMA1_ERROR_MASK                         (0)
#define BSP_CFG_CLMA1MASK (BSP_CLOCKS_CLMA1_ERROR_MASK) /* CLMA1 error mask */
#define BSP_PRV_PRCR_KEY                                    (0xA500U)
#define BSP_PRV_PRCR_CGC_UNLOCK                             ((BSP_PRV_PRCR_KEY) | 0x1U)
#define BSP_PRV_LOCO_STABILIZATION_COUNT                    (40000)
#define BSP_PRV_STARTUP_HIZCTRLEN                           ((BSP_CFG_CLMA1MASK << 2) | (BSP_CFG_CLMA0MASK << 1) | \
                                                             BSP_CFG_CLMA3MASK)

#define BSP_PRV_PCMD_KEY                                    (0xA5U)
/* Command sequence for enabling CLMA. */
#define BSP_PRV_CTL0_ENABLE_TARGET_CMD                      (0x01)
#define BSP_PRV_CTL0_ENABLE_REVERSED_CMD                    (0xFE)


#define BSP_PRV_PRCR_KEY                                    (0xA500U)
#define BSP_PRV_PRCR_CGC_UNLOCK                             ((BSP_PRV_PRCR_KEY) | 0x1U)
#define BSP_PRV_PRCR_LOCK                                   ((BSP_PRV_PRCR_KEY) | 0x0U)


/* Frequencies of clocks. */
#define BSP_PRV_CPU_FREQ_800_MHZ                            (800000000U) // CPU frequency is 800 MHz
#define BSP_PRV_CPU_FREQ_600_MHZ                            (600000000U) // CPU frequency is 600 MHz

/* LOCO enable options. */
#define BSP_CLOCKS_LOCO_DISABLE                             (0)    // LOCO Stop
#define BSP_CLOCKS_LOCO_ENABLE                              (1)    // LOCO Run

#define BSP_CFG_CLOCKS_SECURE (0)
#define BSP_CFG_CLOCKS_OVERRIDE (0)
#define BSP_CFG_MAIN_CLOCK_HZ (25000000) /* Main Clock: 25MHz */
#define BSP_CFG_LOCO_ENABLE (BSP_CLOCKS_LOCO_ENABLE) /* LOCO Enabled */
#define BSP_CFG_PLL1 (BSP_CLOCKS_PLL1_INITIAL) /* PLL1 is initial state */
#define BSP_CFG_PHYSEL (BSP_CLOCKS_PHYSEL_PLL1_DIV) /* Ethernet Clock src: PLL1 divider clock */
#define BSP_CFG_CLMA0_ENABLE (BSP_CLOCKS_CLMA0_ENABLE) /* CLMA0 Enabled */
#define BSP_CFG_CLMA0MASK (BSP_CLOCKS_CLMA0_ERROR_NOT_MASK) /* CLMA0 error not mask */
#define BSP_CFG_CLMA3MASK (BSP_CLOCKS_CLMA3_ERROR_NOT_MASK) /* CLMA3 error not mask */
#define BSP_CFG_CLMA1MASK (BSP_CLOCKS_CLMA1_ERROR_MASK) /* CLMA1 error mask */
#define BSP_CFG_CLMA3_ENABLE (BSP_CLOCKS_CLMA3_ENABLE) /* CLMA3 Enabled */
#define BSP_CFG_CLMA1_ENABLE (BSP_CLOCKS_CLMA1_ENABLE) /* CLMA1 Enabled */
#define BSP_CFG_CLMA2_ENABLE (BSP_CLOCKS_CLMA2_ENABLE) /* CLMA2 Enabled */
#define BSP_CFG_CLMA0_CMPL (1) /* CLMA0 CMPL 1 */
#define BSP_CFG_CLMA1_CMPL (1) /* CLMA1 CMPL 1 */
#define BSP_CFG_CLMA2_CMPL (1) /* CLMA2 CMPL 1 */
#define BSP_CFG_CLMA3_CMPL (1) /* CLMA3 CMPL 1 */
#define BSP_CFG_CLMASEL (BSP_CLOCKS_CLMASEL_LOCO) /* Alternative clock: LOCO */
#define BSP_CFG_CLMA0_CMPH (1023) /* CLMA0 CMPH 1023 */
#define BSP_CFG_CLMA1_CMPH (1023) /* CLMA1 CMPH 1023 */
#define BSP_CFG_CLMA2_CMPH (1023) /* CLMA2 CMPH 1023 */
#define BSP_CFG_CLMA3_CMPH (1023) /* CLMA3 CMPH 1023 */
#define BSP_CFG_DIVSELSUB (BSP_CLOCKS_DIVSELSUB_0) /* ICLK 200MHz */
#define BSP_CFG_SCI0ASYNCCLK (BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SCI0ASYNCCLK: 96MHz */
#define BSP_CFG_SCI1ASYNCCLK (BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SCI1ASYNCCLK: 96MHz */
#define BSP_CFG_SCI2ASYNCCLK (BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SCI2ASYNCCLK: 96MHz */
#define BSP_CFG_SCI3ASYNCCLK (BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SCI3ASYNCCLK: 96MHz */
#define BSP_CFG_SCI4ASYNCCLK (BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SCI4ASYNCCLK: 96MHz */
#define BSP_CFG_SCI5ASYNCCLK (BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SCI5ASYNCCLK: 96MHz */
#define BSP_CFG_SPI0ASYNCCLK (BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SPI0ASYNCCLK: 96MHz */
#define BSP_CFG_SPI1ASYNCCLK (BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SPI1ASYNCCLK: 96MHz */
#define BSP_CFG_SPI2ASYNCCLK (BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SPI2ASYNCCLK: 96MHz */
#define BSP_CFG_SPI3ASYNCCLK (BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ) /* SPI3ASYNCCLK: 96MHz */
#define BSP_CFG_FSELCPU0 (BSP_CLOCKS_FSELCPU0_ICLK_MUL4) /* CPU0CLK Mul: ICLKx4 */
#define BSP_CFG_FSELCPU1 (BSP_CLOCKS_FSELCPU1_ICLK_MUL1) /* CPU1CLK Mul: ICLKx1 */
#define BSP_CFG_CKIO (BSP_CLOCKS_CKIO_ICLK_DIV4) /* CKIO Div: ICLK/4 */
#define BSP_CFG_FSELCANFD (BSP_CLOCKS_CANFD_CLOCK_40_MHZ) /* PCLKCAN 40MHz */
#define BSP_CFG_FSELXSPI0_DIVSELXSPI0 (BSP_CLOCKS_XSPI0_CLOCK_DIV0_12_5_MHZ) /* XSPI_CLK0 12.5MHz */
#define BSP_CFG_FSELXSPI1_DIVSELXSPI1 (BSP_CLOCKS_XSPI1_CLOCK_DIV0_12_5_MHZ) /* XSPI_CLK1 12.5MHz */



/* Calculate the value to write to SCKCR. */
#define BSP_PRV_STARTUP_SCKCR_FSELXSPI0_DIVSELXSPI0_BITS    (BSP_CFG_FSELXSPI0_DIVSELXSPI0 & 0x47U)
#define BSP_PRV_STARTUP_SCKCR_FSELXSPI1_DIVSELXSPI1_BITS    ((BSP_CFG_FSELXSPI1_DIVSELXSPI1 & 0x47U) << 8U)
#define BSP_PRV_STARTUP_SCKCR_CKIO_BITS                     ((BSP_CFG_CKIO & 7U) << 16U)
#define BSP_PRV_STARTUP_SCKCR_FSELCANFD_BITS                ((BSP_CFG_FSELCANFD & 1U) << 20U)
#define BSP_PRV_STARTUP_SCKCR_PHYSEL_BITS                   ((BSP_CFG_PHYSEL & 1U) << 21U)
#define BSP_PRV_STARTUP_SCKCR_CLMASEL_BITS                  ((BSP_CFG_CLMASEL & 1U) << 22U)
#define BSP_PRV_STARTUP_SCKCR_SPI0ASYNCSEL_BITS             ((BSP_CFG_SPI0ASYNCCLK & 1U) << 24U)
#define BSP_PRV_STARTUP_SCKCR_SPI1ASYNCSEL_BITS             ((BSP_CFG_SPI1ASYNCCLK & 1U) << 25U)
#define BSP_PRV_STARTUP_SCKCR_SPI2ASYNCSEL_BITS             ((BSP_CFG_SPI2ASYNCCLK & 1U) << 26U)
#define BSP_PRV_STARTUP_SCKCR_SCI0ASYNCSEL_BITS             ((BSP_CFG_SCI0ASYNCCLK & 1U) << 27U)
#define BSP_PRV_STARTUP_SCKCR_SCI1ASYNCSEL_BITS             ((BSP_CFG_SCI1ASYNCCLK & 1U) << 28U)
#define BSP_PRV_STARTUP_SCKCR_SCI2ASYNCSEL_BITS             ((BSP_CFG_SCI2ASYNCCLK & 1U) << 29U)
#define BSP_PRV_STARTUP_SCKCR_SCI3ASYNCSEL_BITS             ((BSP_CFG_SCI3ASYNCCLK & 1U) << 30U)
#define BSP_PRV_STARTUP_SCKCR_SCI4ASYNCSEL_BITS             ((BSP_CFG_SCI4ASYNCCLK & 1U) << 31U)


/* Calculate the value to write to SCKCR2. */
#define BSP_PRV_STARTUP_SCKCR2_FSELCPU0_BITS                (BSP_CFG_FSELCPU0 & 3U)
#define BSP_PRV_STARTUP_SCKCR2_FSELCPU1_BITS                ((BSP_CFG_FSELCPU1 & 3U) << 2U)
#define BSP_PRV_STARTUP_SCKCR2_RESERVED_BIT4_BITS           (1U << 4U) // The write value should be 1.
#define BSP_PRV_STARTUP_SCKCR2_DIVSELSUB_BITS               ((BSP_CFG_DIVSELSUB & 1U) << 5U)
#define BSP_PRV_STARTUP_SCKCR2_SPI3ASYNCSEL_BITS            ((BSP_CFG_SPI3ASYNCCLK & 1U) << 24U)
#define BSP_PRV_STARTUP_SCKCR2_SCI5ASYNCSEL_BITS            ((BSP_CFG_SCI5ASYNCCLK & 1U) << 25U)


#define BSP_PRV_STARTUP_SCKCR                               (BSP_PRV_STARTUP_SCKCR_FSELXSPI0_DIVSELXSPI0_BITS | \
                                                             BSP_PRV_STARTUP_SCKCR_FSELXSPI1_DIVSELXSPI1_BITS | \
                                                             BSP_PRV_STARTUP_SCKCR_CKIO_BITS |                  \
                                                             BSP_PRV_STARTUP_SCKCR_FSELCANFD_BITS |             \
                                                             BSP_PRV_STARTUP_SCKCR_PHYSEL_BITS |                \
                                                             BSP_PRV_STARTUP_SCKCR_CLMASEL_BITS |               \
                                                             BSP_PRV_STARTUP_SCKCR_SPI0ASYNCSEL_BITS |          \
                                                             BSP_PRV_STARTUP_SCKCR_SPI1ASYNCSEL_BITS |          \
                                                             BSP_PRV_STARTUP_SCKCR_SPI2ASYNCSEL_BITS |          \
                                                             BSP_PRV_STARTUP_SCKCR_SCI0ASYNCSEL_BITS |          \
                                                             BSP_PRV_STARTUP_SCKCR_SCI1ASYNCSEL_BITS |          \
                                                             BSP_PRV_STARTUP_SCKCR_SCI2ASYNCSEL_BITS |          \
                                                             BSP_PRV_STARTUP_SCKCR_SCI3ASYNCSEL_BITS |          \
                                                             BSP_PRV_STARTUP_SCKCR_SCI4ASYNCSEL_BITS)

#define BSP_PRV_STARTUP_SCKCR2                              (BSP_PRV_STARTUP_SCKCR2_FSELCPU0_BITS |      \
                                                             BSP_PRV_STARTUP_SCKCR2_FSELCPU1_BITS |      \
                                                             BSP_PRV_STARTUP_SCKCR2_RESERVED_BIT4_BITS | \
                                                             BSP_PRV_STARTUP_SCKCR2_DIVSELSUB_BITS |     \
                                                             BSP_PRV_STARTUP_SCKCR2_SPI3ASYNCSEL_BITS |  \
                                                             BSP_PRV_STARTUP_SCKCR2_SCI5ASYNCSEL_BITS)


#define BSP_PRV_STARTUP_SCKCR2_FSELCPU0_ICLK_MUL2           (BSP_CLOCKS_FSELCPU0_ICLK_MUL2 << \
                                                             R_SYSC_S_SCKCR2_FSELCPU0_Pos)
#define BSP_PRV_STARTUP_SCKCR2_FSELCPU1_ICLK_MUL2           (BSP_CLOCKS_FSELCPU1_ICLK_MUL2 << \
                                                             R_SYSC_S_SCKCR2_FSELCPU1_Pos)

#define FSP_PARAMETER_NOT_USED(p)    (void) ((p))

void temporaliy_set_system_clock (uint32_t sckcr2)
{
    volatile uint32_t dummy            = 0;
    uint32_t          sckcr2_cpu_clock = R_SYSC_S->SCKCR2;

    /* Check if FSELCPU0 bit of sckcr2 variable is 00b and CPU0 clock is 800MHz. (Or 600MHz) */
    if (!(BSP_PRV_SCKCR2_FSELCPU0_MASK & sckcr2))
    {
        /* Set FSELCPU0 bit to 01b and CPU0 clock is 400MHz. (Or 300MHz) */
        sckcr2_cpu_clock =
            ((sckcr2_cpu_clock & ~R_SYSC_S_SCKCR2_FSELCPU0_Msk) | BSP_PRV_STARTUP_SCKCR2_FSELCPU0_ICLK_MUL2);
    }

    /* Check if FSELCPU1 bit of sckcr2 variable is 00b and CPU1 clock is 800MHz. (Or 600MHz) */
    if (!(BSP_PRV_SCKCR2_FSELCPU1_MASK & sckcr2))
    {
        /* Set FSELCPU1 bit to 01b and CPU1 clock is 400MHz. (Or 300MHz) */
        sckcr2_cpu_clock =
            ((sckcr2_cpu_clock & ~R_SYSC_S_SCKCR2_FSELCPU1_Msk) | BSP_PRV_STARTUP_SCKCR2_FSELCPU1_ICLK_MUL2);
    }

    /* Temporarily set system clock. */
    if (R_SYSC_S->SCKCR2 != sckcr2_cpu_clock)
    {
        R_SYSC_S->SCKCR2 = sckcr2_cpu_clock;

        /** In order to secure processing after clock frequency is changed,
         *  dummy read the same register at least eight times.
         *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
        dummy = R_SYSC_S->SCKCR2;
        dummy = R_SYSC_S->SCKCR2;
        dummy = R_SYSC_S->SCKCR2;
        dummy = R_SYSC_S->SCKCR2;
        dummy = R_SYSC_S->SCKCR2;
        dummy = R_SYSC_S->SCKCR2;
        dummy = R_SYSC_S->SCKCR2;
        dummy = R_SYSC_S->SCKCR2;
    }

    FSP_PARAMETER_NOT_USED(dummy);
}


uint32_t SystemCoreClock = 0U;

void system_core_clock_update (void)
{
    uint32_t devselsub = R_SYSC_S->SCKCR2_b.DIVSELSUB;
    uint32_t fselcpu = R_SYSC_S->SCKCR2_b.FSELCPU0;

    if (0U == devselsub)
    {
        SystemCoreClock = BSP_PRV_CPU_FREQ_800_MHZ >> fselcpu;
    }
    else
    {
        SystemCoreClock = BSP_PRV_CPU_FREQ_600_MHZ >> fselcpu;
    }
}


static void set_hard_reset (void)
{
    volatile uint32_t dummy;
    uint32_t          sckcr  = BSP_PRV_STARTUP_SCKCR & BSP_PRV_SCKCR_MASK;
    uint32_t          sckcr2 = BSP_PRV_STARTUP_SCKCR2 & BSP_PRV_SCKCR2_MASK;

    /* Note that if switching CPU clock to 800MHz, switch to 400MHz and then to 800MHz.
     * The same applies to changing the clock frequency when the bus reference clock is 150MHz.
     * This is the case if FSELCPUn bit of sckcr2 variable is 00b(CPU clock is 800MHz or 600MHz). */
    if (!(BSP_PRV_SCKCR2_FSELCPU0_MASK & sckcr2) || !(BSP_PRV_SCKCR2_FSELCPU1_MASK & sckcr2))
    {
        temporaliy_set_system_clock(sckcr2);
    }

    /* Set the system source clock */
    R_SYSC_S->SCKCR2 = sckcr2;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;

    R_SYSC_NS->SCKCR = sckcr;

    R_SYSC_NS->MSTPCRC_b.MSTPCRC00 = 0; //JOAO

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;

    FSP_PARAMETER_NOT_USED(dummy);

    /* Clock is now at requested frequency. */

    /* Update the CMSIS core clock variable so that it reflects the new ICLK frequency. */
    system_core_clock_update();
}


void clock_init (void)
{    
     volatile uint32_t dummy = 0;

    /* Unlock CGC protection registers. */
    R_RWP_NS->PRCRN = (uint16_t) BSP_PRV_PRCR_CGC_UNLOCK;
    R_RWP_S->PRCRS  = (uint16_t) BSP_PRV_PRCR_CGC_UNLOCK;

    /* The SystemCoreClock needs to be updated before calling R_BSP_SoftwareDelay. */
    system_core_clock_update();   
    
    
    
    set_hard_reset();
    
    R_SYSC_S->LOCOCR = BSP_CLOCKS_LOCO_ENABLE;

    /* Only start using the LOCO clock after
     * the LOCO oscillation stabilization time (tLOCOWT) has elapsed. */
    for (uint16_t i = 0; i < BSP_PRV_LOCO_STABILIZATION_COUNT; i++)
    {
        __asm volatile ("nop");
    }


    R_SYSC_S->HIZCTRLEN = BSP_PRV_STARTUP_HIZCTRLEN;


    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA0->CMPL = BSP_CFG_CLMA0_CMPL;
    R_CLMA0->CMPH = BSP_CFG_CLMA0_CMPH;

    /* Enabling CLMA0 operation. */
    do
    {
        R_CLMA0->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA0->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA0->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA0->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA0->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA0->PROTSR;
        }
    } while (1 == R_CLMA0->PROTSR_b.PRERR);


    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA1->CMPL = BSP_CFG_CLMA1_CMPL;
    R_CLMA1->CMPH = BSP_CFG_CLMA1_CMPH;

    /* Enabling CLMA1 operation. */
    do
    {
        R_CLMA1->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA1->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA1->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA1->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA1->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA1->PROTSR;
        }
    } while (1 == R_CLMA1->PROTSR_b.PRERR);


    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA2->CMPL = BSP_CFG_CLMA2_CMPL;
    R_CLMA2->CMPH = BSP_CFG_CLMA2_CMPH;

    /* Enabling CLMA2 operation. */
    do
    {
        R_CLMA2->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA2->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA2->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA2->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA2->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA2->PROTSR;
        }
    } while (1 == R_CLMA2->PROTSR_b.PRERR);


    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA3->CMPL = BSP_CFG_CLMA3_CMPL;
    R_CLMA3->CMPH = BSP_CFG_CLMA3_CMPH;

    /* Enabling CLMA3 operation. */
    do
    {
        R_CLMA3->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA3->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA3->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA3->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA3->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA3->PROTSR;
        }
    } while (1 == R_CLMA3->PROTSR_b.PRERR);

    /* Lock CGC and LPM protection registers. */
    R_RWP_NS->PRCRN = (uint16_t) BSP_PRV_PRCR_LOCK;
    R_RWP_S->PRCRS  = (uint16_t) BSP_PRV_PRCR_LOCK;

    FSP_PARAMETER_NOT_USED(dummy);
}









