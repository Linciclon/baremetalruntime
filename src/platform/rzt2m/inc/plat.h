#ifndef PLAT_H
#define PLAT_H

#define PLAT_RZT2M 1


/* On-chip system RAM used by the native image. */
#ifndef RUN_INSIDE_VM
#define PLAT_MEM_BASE 0x10000000
#else
#define PLAT_MEM_BASE  0x10100000
#endif
#define PLAT_MEM_SIZE  0x30000

#ifdef SINGLE_CORE
#define PLAT_CPU_COUNT 1
#else
#define PLAT_CPU_COUNT 2
#endif
#define STACK_SIZE     0x4000
#define PLAT_TOTAL_STACK_SIZE (2 * STACK_SIZE * PLAT_CPU_COUNT)

#define PLAT_GENERIC_TIMER_CNTCTL_BASE  0xC0060000
#define PLAT_GENERIC_TIMER_FREQ         25000000

/* The MTU block is clocked by PCLKH.  Use the same overflow sources as the
 * known-good bare-metal application: TCIV4 or TCIV7. */
#define PLAT_TIMER_FREQ                 200000000U

#ifndef MTU_CHANNEL
#define MTU_CHANNEL                     4
#endif

#if MTU_CHANNEL == 4
#define RZT2M_TIMER_EVENT_ID            93U
#define RZT2M_TIMER_COUNTER_ADDR         0x90001212U
#elif MTU_CHANNEL == 7
#define RZT2M_TIMER_EVENT_ID            106U
#define RZT2M_TIMER_COUNTER_ADDR         0x90001A12U
#else
#error "Unsupported MTU_CHANNEL; supported channels are 4 and 7"
#endif

#define RZT2M_TIMER_IRQ_ID              (RZT2M_TIMER_EVENT_ID + 32U)

#define PLAT_GICD_BASE_ADDR 0x94000000
#define PLAT_GICR_BASE_ADDR 0x94100000

/* UART/SCI channel used by this RZ/T2M image (valid channels: 0 to 5). */
#ifndef RZT2M_UART_CHANNEL
#define RZT2M_UART_CHANNEL 0
#endif

#if RZT2M_UART_CHANNEL == 0
#define USE_UART0 1
#elif RZT2M_UART_CHANNEL == 1
#define USE_UART1 1
#elif RZT2M_UART_CHANNEL == 2
#define USE_UART2 1
#elif RZT2M_UART_CHANNEL == 3
#define USE_UART3 1
#elif RZT2M_UART_CHANNEL == 4
#define USE_UART4 1
#elif RZT2M_UART_CHANNEL == 5
#define USE_UART5 1
#else
#error "RZT2M_UART_CHANNEL must be between 0 and 5"
#endif

/* RXI0 is SPI 321 and each SCI channel occupies four consecutive SPIs. */
#define UART_IRQ_ID (321U + (4U * RZT2M_UART_CHANNEL))


#endif
