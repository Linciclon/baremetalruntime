#ifndef PLAT_H
#define PLAT_H

#include <sysregs.h>
#include <iocon.h>
#include <syscon.h>

//Secure memory and peripheral regions

#define PLAT_NON_UNIFIED_MEM

#define PLAT_CODE_MEM_BASE  0x10000000
#define PLAT_CODE_MEM_SIZE  0x10000

#define PLAT_NSC_BASE       0x1000F000
#define PLAT_NSC_SIZE       0x1000
#define PLAT_NSC_DWT_PROBE_ADDR PLAT_NSC_BASE

#define PLAT_MEM_BASE       0x30000000
#define PLAT_MEM_SIZE       0x8000

#define PLAT_UART_ADDR      0x40088000UL
#define PLAT_UART_SIZE      0x1000UL
#define UART_IRQ_ID         16+EXT_INT_BASE

#define STACK_SIZE          0x4000

#define PLAT_TIMER_FREQ     150000000ULL
#define PLAT_DWT_FREQ       150000000ULL

#define PLAT_MAX_INTERRUPTS 75

#define UART_RX_PORT        port1
#define UART_RX_PIN         pin24

#define UART_TX_PORT        port0
#define UART_TX_PIN         pin27

// Non-secure memory and peripheral regions
// Non-secure code memory is located at 0x20000


#define PLAT_NS_CODE_MEM_BASE  0x20000
#define PLAT_NS_VECTOR_TABLE_BASE  0x23000
#define PLAT_NS_CODE_MEM_SIZE  0x10000

#define PLAT_NS_MEM_BASE       0x20010000
#define PLAT_NS_MEM_SIZE       0x8000

#define PLAT_NS_PERIPH_BASE    0x40000000UL
#define PLAT_NS_PERIPH_SIZE    0x20000000UL


#define PLAT_NS_UART_ADDR      0x40089000UL
#define PLAT_NS_UART_SIZE      0x1000UL

#endif
