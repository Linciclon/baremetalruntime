#include <sysregs.h>
#include <plat.h>

.text

.balign 0x20
.global _exception_vector
_exception_vector:
    b .
    b .
    b .
    b .
    b .
    b .
    b irq_handler

irq_handler:
#if defined(PLAT_RZT2M) && defined(USINGTIMER) && \
    defined(NONSECURE_INTERRUPT_LATENCY)
    push {r0-r2}
    ldr r0, =RZT2M_TIMER_COUNTER_ADDR
    ldrh r0, [r0]
    ldr r1, =nonsecure_systick_latency_cycles
    str r0, [r1]
    ldr r1, =nonsecure_systick_sample_count
    ldr r2, [r1]
    add r2, r2, #1
    str r2, [r1]
    push {r3-r12, r14}
#else
    push {r0-r12, r14}
#endif
    bl gic_handle
#if defined(PLAT_RZT2M) && defined(USINGTIMER) && \
    defined(NONSECURE_INTERRUPT_LATENCY)
    pop {r3-r12, r14}
    pop {r0-r2}
#else
    pop {r0-r12, r14}
#endif
    SUBS PC, lr, #4
