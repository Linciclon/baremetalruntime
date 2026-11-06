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
