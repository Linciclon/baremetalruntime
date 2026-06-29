/**
 * SPDX-License-Identifier: Apache-2.0U
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef ARCH_SYSREGS_H
#define ARCH_SYSREGS_H

#ifndef __ASSEMBLER__

#include <core.h>

#endif /* __ASSEMBLER__ */

/* System Control Block */
#define SCB_BASE                 (0xE000ED00UL)
#define SCB_BASE_NS              (0xE002ED00UL)

#define SCB_VTOR_OFF             (8)
#define SCB_SHCSR_OFF            (36)


#define SCB_SHCSR_SYSTICKENA     (1UL << 1UL)
#define SCB_SHCSR_MEMFAULTENA    (1UL << 16UL)
#define SCB_SHCSR_BUSFAULTENA    (1UL << 17UL)
#define SCB_SHCSR_USGFAULTENA    (1UL << 18UL)
#define SCB_SHCSR_SYSTICKACT     (1UL << 11UL)

#define SCB_ICSR_PENDSTSET       (1UL << 26UL)
#define SCB_ICSR_PENDSTCLR       (1UL << 25UL)

#define SCB_SHCSR_EN_ALL_FAULTS                                              \
    (SCB_SHCSR_MEMFAULTENA | SCB_SHCSR_BUSFAULTENA | SCB_SHCSR_USGFAULTENA)

#ifndef __ASSEMBLER__
static struct scb* const scb = (struct scb*)SCB_BASE;
static struct scb* const scb_ns = (struct scb*)SCB_BASE_NS;

struct scb {
    volatile const uint32_t cpuid;
    volatile uint32_t icsr;
    volatile uint32_t vtor;
    volatile uint32_t aircr;
    volatile uint32_t scr;
    volatile uint32_t ccr;
    volatile uint32_t shpr1;    // [7:0] MemManage | [15:8] Bus Fault | [23:16] UsageFault
    volatile uint32_t shpr2;    // [31:24] SVCall
    volatile uint32_t shpr3;    // [7:0] DebugMonitor | [23:16] PendSV | [31:24] SysTick
    volatile uint32_t shcsr;
    volatile uint32_t cfsr;
    volatile uint32_t hfsr;
    volatile uint32_t dfsr;
    volatile uint32_t mmfar;
    volatile uint32_t bfar;
    volatile uint32_t afsr;
    volatile const uint32_t id_pfr0;
    volatile const uint32_t id_pfr1;
    volatile const uint32_t id_dfr0;
    volatile const uint32_t id_afr0;
    volatile const uint32_t id_mmfr0;
    volatile const uint32_t id_mmfr1;
    volatile const uint32_t id_mmfr2;
    volatile const uint32_t id_mmfr3;
    volatile const uint32_t id_isar0;
    volatile const uint32_t id_isar1;
    volatile const uint32_t id_isar2;
    volatile const uint32_t id_isar3;
    volatile const uint32_t id_isar4;
    volatile const uint32_t id_isar5;
    volatile const uint32_t clidr;
    volatile const uint32_t ctr;
    volatile uint32_t ccsidr;
    volatile uint32_t csselr;
    volatile uint32_t cpacr;
};

#endif

/* SysTick */
#define SYSTICK_BASE          (0xE000E010UL)
#define SYSTICK_NS_BASE       (SYSTICK_BASE + NS_ALIAS_OFFSET)

#define SYSTICK_CSR_ENABLE    (1UL << 0UL)
#define SYSTICK_CSR_TICKINT   (1UL << 1UL)
#define SYSTICK_CSR_CLKSOURCE (1UL << 2UL)

#ifndef __ASSEMBLER__
static struct systick* const systick = (struct systick*)SYSTICK_BASE;

struct systick {
    volatile uint32_t csr;          //CTRL 
                                        // bit 0  ENABLE      SysTick enable
                                        // bit 1  TICKINT     SysTick exception enable
                                        // bit 2  CLKSOURCE   Clock source
                                        // bit 16 COUNTFLAG   Set quando o contador chegou a 0
    volatile uint32_t rvr;          //LOAD
    volatile uint32_t cvr;          //VAL
    volatile const uint32_t calib;  //CALIB
};
#endif /* |__ASSEMBLER__ */

/* Nested Vectored Interrupt Controller */
#define NVIC_BASE           (0xE000E100UL)
#define NVIC_NS_BASE        (NVIC_BASE + NS_ALIAS_OFFSET)

#define NVIC_MAX_INTERRUPTS (496U)
#ifndef PLAT_NVIC_MAX_INTERRUPTS
#define PLAT_NVIC_MAX_INTERRUPTS NVIC_MAX_INTERRUPTS
#endif

#define NVIC_PRIO_BITS 3U // Number of priority bits implemented in the NVIC

#ifndef __ASSEMBLER__
static struct nvic* const nvic = (struct nvic*)NVIC_BASE;

enum exc_numbers {
    EXC_RESET = 1,
    EXC_NMI = 2,
    EXC_HARD_FAULT = 3,
    EXC_MEM_MANAGE = 4,
    EXC_BUS_FAULT = 5,
    EXC_USAGE_FAULT = 6,
    EXC_SVCALL = 11,
    EXC_DEBUG_MON = 12,
    EXC_PENDSV = 14,
    EXC_SYSTICK = 15,
    EXT_INT_BASE = 16,
};

struct nvic {
    volatile uint32_t iser[16]; // 0x000 + x*4: Interrupt Set-Enable Register x
    uint32_t reserved0[16];
    volatile uint32_t icer[16]; // 0x080 + x*4: Interrupt Clear-Enable Register x
    uint32_t reserved1[16];
    volatile uint32_t ispr[16]; // 0x100 + x*4: Interrupt Set-Pending Register x
    uint32_t reserved2[16];
    volatile uint32_t icpr[16]; // 0x180 + x*4: Interrupt Clear-Pending Register x
    uint32_t reserved3[16];
    volatile uint32_t iabr[16]; // 0x200 + x*4: Interrupt Active Bit Register x
    uint32_t reserved4[16];
    volatile uint32_t itns[16]; // 0x280 + x*4: Interrupt Target Non-Secure Register x
    uint32_t reserved5[16];
    volatile uint32_t ipr[496]; // 0x300 + x*4: Interrupt Priority Register x
    uint32_t reserved6[580];
    volatile uint32_t stir;     // 0xe00: Software Trigger Interrupt Register
};

static inline void arm_unmask_irq(void)
{
    asm volatile("cpsie i");
}
#endif /* |__ASSEMBLER__ */

#endif /* ARCH_SYSREGS_H */
