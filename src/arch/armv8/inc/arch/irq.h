#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include <plat.h>

#define IPI_IRQ_ID (0)
#ifdef USINGTIMER
#define TIMER_IRQ_ID (RZT2M_TIMER_IRQ_ID)
#else
#define TIMER_IRQ_ID (27)
#endif
#define IRQ_NUM (1024)
#define IRQ_MAX_PRIO (0)

#endif /* ARCH_IRQ_H */
