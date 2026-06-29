#ifndef PLAT_H
#define PLAT_H

#include <sysregs.h>
#include <iocon.h>
#include <syscon.h>

#define PLAT_NON_UNIFIED_MEM

#define PLAT_CODE_MEM_BASE  0x20000
#define PLAT_CODE_MEM_SIZE  0x10000

#define PLAT_NSC_DWT_PROBE_ADDR 0x1000F000

#define PLAT_MEM_BASE       0x20010000
#define PLAT_MEM_SIZE       0x8000

#define STACK_SIZE          0x4000

#define PLAT_TIMER_FREQ     15000000ULL
#define PLAT_DWT_FREQ       150000000ULL
                            
#define PLAT_UART_ADDR      0x40089000UL
#define UART_IRQ_ID         17+EXT_INT_BASE

#define UART_RX_PORT        port0
#define UART_RX_PIN         pin03

#define UART_TX_PORT        port0
#define UART_TX_PIN         pin02

#define FCRST               FC3_RST
#define FCCLKSEL            FCCLKSEL3
#define SYSCON_AHBCLKTRL1   SYSCON_AHBCLKTRL1_FC3

#define PLAT_MAX_INTERRUPTS 75

#endif
