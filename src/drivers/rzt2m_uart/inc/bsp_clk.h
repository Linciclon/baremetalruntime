
#ifndef BSP_CLOCKS_H
#define BSP_CLOCKS_H


#define BSP_CLOCKS_XSPI0_CLOCK_DIV0_12_5_MHZ                (0x06) // xSPI0 base clock 800MHz and xSPI0 clock 12.5 MHz.

/* xSPI unit1 clock options. */
#define BSP_CLOCKS_XSPI1_CLOCK_DIV0_12_5_MHZ                (0x06) // xSPI1 base clock 800MHz and xSPI1 clock 12.5 MHz.


/* CKIO clock options. */

#define BSP_CLOCKS_CKIO_ICLK_DIV4                           (2)    // CKIO clock 50.0 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                   // or 37.5 MHz  (when SCKCR2.DIVSELSUB = 1).

/* CANFD clock options. */
#define BSP_CLOCKS_CANFD_CLOCK_40_MHZ                       (1)    // CANFD clock 40 MHz.

/* Ethernet PHY reference clock (ETHn_REFCLK : n = 0 to 2) options. */
#define BSP_CLOCKS_PHYSEL_PLL1_DIV                          (0)    // PLL1 devider clock.

/* Alternative clock options when main clock abnormal oscillation is detected in CLMA3. */
#define BSP_CLOCKS_CLMASEL_LOCO                             (0)    // LOCO clock.

/* SPI clock options. */
#define BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SPI0 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SPI1 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SPI2 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SPI3 asynchronous serial clock 96.0 MHz.

/* SCI clock options. */
#define BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SCI0 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SCI1 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SCI2 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SCI3 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SCI4 asynchronous serial clock 96.0 MHz.
#define BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ    (1)    // SCI5 asynchronous serial clock 96.0 MHz.

/* CPU0 clock options. */
#define BSP_CLOCKS_FSELCPU0_ICLK_MUL4                       (0)    // CPU0 clock 800 MHz (when SCKCR2.DIVSELSUB = 0),
                                                                   // or 600 MHz (when SCKCR2.DIVSELSUB = 1).
#define BSP_CLOCKS_FSELCPU0_ICLK_MUL2                       (1)    // CPU0 clock 400 MHz (when SCKCR2.DIVSELSUB = 0),
                                                                   // or 300 MHz (when SCKCR2.DIVSELSUB = 1).


/* CPU1 clock options. */
#define BSP_CLOCKS_FSELCPU1_ICLK_MUL2                       (1)    // CPU1 clock 400 MHz (when SCKCR2.DIVSELSUB = 0),
                                                                   // or 300 MHz (when SCKCR2.DIVSELSUB = 1).
#define BSP_CLOCKS_FSELCPU1_ICLK_MUL1                       (2)    // CPU1 clock 200 MHz (when SCKCR2.DIVSELSUB = 0),
                                                                   // or 150 MHz (when SCKCR2.DIVSELSUB = 1).

/* Peripheral module base clock options. */
#define BSP_CLOCKS_DIVSELSUB_0                              (0)    // ICLK:200MHz, PCLKH:200MHz, PCLKM:100MHz,
                                                                   // PCLKL:50MHz, PCLKADC:25MHz, PCLKGPTL:400MHz.

/* LOCO enable options. */
#define BSP_CLOCKS_LOCO_DISABLE                             (0)    // LOCO Stop
#define BSP_CLOCKS_LOCO_ENABLE                              (1)    // LOCO Run

/* PLL1 enable options. */
#define BSP_CLOCKS_PLL1_INITIAL                             (0xFF) // Initial (This value should not be reflected in the register)

/* CLMA error mask options. */
#define BSP_CLOCKS_CLMA0_ERROR_NOT_MASK                     (1)    // CLMA0 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA1_ERROR_MASK                         (0)    // CLMA1 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA1_ERROR_NOT_MASK                     (1)    // CLMA1 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA3_ERROR_MASK                         (0)    // CLMA3 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA3_ERROR_NOT_MASK                     (1)    // CLMA3 error is transferred to POE3 and POEG.

/* CLMA enable options. */
#define BSP_CLOCKS_CLMA0_ENABLE                             (1)    // Enable CLMA0 operation.
#define BSP_CLOCKS_CLMA1_ENABLE                             (1)    // Enable CLMA1 operation.
#define BSP_CLOCKS_CLMA2_ENABLE                             (1)    // Enable CLMA2 operation.
#define BSP_CLOCKS_CLMA3_ENABLE                             (1)    // Enable CLMA3 operation.

/* Create a mask of valid bits in SCKCR. */
#define BSP_PRV_SCKCR_FSELXSPI0_MASK                        (7U << 0)
#define BSP_PRV_SCKCR_DIVSELXSPI0_MASK                      (1U << 6)
#define BSP_PRV_SCKCR_FSELXSPI1_MASK                        (7U << 8)
#define BSP_PRV_SCKCR_DIVSELXSPI1_MASK                      (1U << 14)
#define BSP_PRV_SCKCR_CKIO_MASK                             (7U << 16)
#define BSP_PRV_SCKCR_FSELCANFD_MASK                        (1U << 20)
#define BSP_PRV_SCKCR_PHYSEL_MASK                           (1U << 21)
#define BSP_PRV_SCKCR_CLMASEL_MASK                          (1U << 22)
#define BSP_PRV_SCKCR_SPI0ASYNCSEL_MASK                     (1U << 24)
#define BSP_PRV_SCKCR_SPI1ASYNCSEL_MASK                     (1U << 25)
#define BSP_PRV_SCKCR_SPI2ASYNCSEL_MASK                     (1U << 26)
#define BSP_PRV_SCKCR_SCI0ASYNCSEL_MASK                     (1U << 27)
#define BSP_PRV_SCKCR_SCI1ASYNCSEL_MASK                     (1U << 28)
#define BSP_PRV_SCKCR_SCI2ASYNCSEL_MASK                     (1U << 29)
#define BSP_PRV_SCKCR_SCI3ASYNCSEL_MASK                     (1U << 30)
#define BSP_PRV_SCKCR_SCI4ASYNCSEL_MASK                     (1U << 31)
#define BSP_PRV_SCKCR_MASK                                  (((((((((((((((BSP_PRV_SCKCR_FSELXSPI0_MASK |    \
                                                                           BSP_PRV_SCKCR_DIVSELXSPI0_MASK) | \
                                                                          BSP_PRV_SCKCR_FSELXSPI1_MASK) |    \
                                                                         BSP_PRV_SCKCR_DIVSELXSPI1_MASK) |   \
                                                                        BSP_PRV_SCKCR_CKIO_MASK) |           \
                                                                       BSP_PRV_SCKCR_FSELCANFD_MASK) |       \
                                                                      BSP_PRV_SCKCR_PHYSEL_MASK) |           \
                                                                     BSP_PRV_SCKCR_CLMASEL_MASK) |           \
                                                                    BSP_PRV_SCKCR_SPI0ASYNCSEL_MASK) |       \
                                                                   BSP_PRV_SCKCR_SPI1ASYNCSEL_MASK) |        \
                                                                  BSP_PRV_SCKCR_SPI2ASYNCSEL_MASK) |         \
                                                                 BSP_PRV_SCKCR_SCI0ASYNCSEL_MASK) |          \
                                                                BSP_PRV_SCKCR_SCI1ASYNCSEL_MASK) |           \
                                                               BSP_PRV_SCKCR_SCI2ASYNCSEL_MASK) |            \
                                                              BSP_PRV_SCKCR_SCI3ASYNCSEL_MASK) |             \
                                                             BSP_PRV_SCKCR_SCI4ASYNCSEL_MASK)

/* Create a mask of valid bits in SCKCR2. */
#define BSP_PRV_SCKCR2_FSELCPU0_MASK                        (3U << 0)
#define BSP_PRV_SCKCR2_FSELCPU1_MASK                        (3U << 2)
#define BSP_PRV_SCKCR2_RESERVED_BIT4_MASK                   (1U << 4)
#define BSP_PRV_SCKCR2_DIVSELSUB_MASK                       (1U << 5)
#define BSP_PRV_SCKCR2_SPI3ASYNCSEL_MASK                    (1U << 24)
#define BSP_PRV_SCKCR2_SCI5ASYNCSEL_MASK                    (1U << 25)
#define BSP_PRV_SCKCR2_MASK                                 (((((BSP_PRV_SCKCR2_FSELCPU0_MASK |      \
                                                                 BSP_PRV_SCKCR2_FSELCPU1_MASK) |     \
                                                                BSP_PRV_SCKCR2_RESERVED_BIT4_MASK) | \
                                                               BSP_PRV_SCKCR2_DIVSELSUB_MASK) |      \
                                                              BSP_PRV_SCKCR2_SPI3ASYNCSEL_MASK) |    \
                                                             BSP_PRV_SCKCR2_SCI5ASYNCSEL_MASK)



void clock_init(void);             // Used to initiate all the clocks


#endif