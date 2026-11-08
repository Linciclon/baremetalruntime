#ifndef PLAT_H
#define PLAT_H

#define PLAT_RZT2M 1

#ifndef RUN_INSIDE_VM
#define PLAT_MEM_BASE  0x10000000
#else
#define PLAT_MEM_BASE  0x10160000
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

#define PLAT_GICD_BASE_ADDR (0x94000000) //joao
#define PLAT_GICR_BASE_ADDR (0x94100000) //joao

/*
 * UART used by the RZ/T2M platform.
 *
 * Supported values are 2 and 4.  This selection is shared by the platform
 * code and by the RZ/T2M UART driver, so the MMIO address, pin mux and IRQ
 * always refer to the same SCI channel.
 */
#define RZT2M_UART_CHANNEL 4

#if RZT2M_UART_CHANNEL == 2
#define USE_UART2          1
#define UART_IRQ_ID        329 /* RXI2: 297 + GIC SPI offset 32 */
#elif RZT2M_UART_CHANNEL == 4
#define USE_UART4          1
#define UART_IRQ_ID        337 /* RXI4: 305 + GIC SPI offset 32 */
#else
#error "RZT2M_UART_CHANNEL must be 2 or 4"
#endif

//this is shared memory location for the tee client and the tee server, it is used to pass the parameters to the tee server, and also to pass the return values back to the tee client
#define PHYS_SHMEMORY_ADDR 0x10080000
#define SIZE_OF_PHYS_SHMEMORY_ADDR 0x10000

#endif
