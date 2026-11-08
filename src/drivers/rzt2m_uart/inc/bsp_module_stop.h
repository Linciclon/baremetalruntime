#ifndef BSP_MODULE_H
#define BSP_MODULE_H

#include "renesas.h"


#define BSP_MSTP_REG_FSP_IP_SCI(channel)           *((4U >= channel) ? &R_SYSC_NS->MSTPCRA : &R_SYSC_S->MSTPCRG)
#define BSP_MSTP_BIT_FSP_IP_SCI(channel)           ((4U >= channel) ? (1U << (8U + channel)) : (1U));
#define BSP_MSTP_DMY_FSP_IP_SCI(channel)           (0 >= channel) ? R_SCI0->RDR : ((1 >= channel) ? R_SCI1->RDR :         \
                                                                                   ((2 >=                                 \
                                                                                     channel) ? R_SCI2->RDR : ((3 >=      \
                                                                                                                channel)  \
                                                                                                               ? R_SCI3   \
                                                                                                               ->RDR :    \
                                                                                                               ((4        \
                                                                                                                 >=       \
                                                                                                                 channel) \
                                                                                                                ? R_SCI4  \
                                                                                                                ->RDR :   \
                                                                                                                R_SCI5->  \
                                                                                                                RDR))))

#define BSP_MSTP_REG_FSP_IP_MTU3(channel)          R_SYSC_NS->MSTPCRC
#define BSP_MSTP_BIT_FSP_IP_MTU3(channel)          (1U);
#define BSP_MSTP_DMY_FSP_IP_MTU3(channel)          R_MTU0->TCR;

#endif
