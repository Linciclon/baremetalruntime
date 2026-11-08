/* SPDX-License-Identifier: Apache-2.0 */
#ifndef __INCLUDE_MTOWER_CROSSHYP_H
#define __INCLUDE_MTOWER_CROSSHYP_H

#include <stdint.h>

#define MTOWER1             ((uintptr_t)0x1UL)
#define MTOWER2             ((uintptr_t)0x2UL)

#define CROSSCON_HC_BLNS_ID ((uintptr_t)0x0UL)
#define CROSSCON_HC_SG_ID   crosscon_hc_sg_id
#define CROSSCON_HC_BOOT    ((uintptr_t)0xfUL)

extern uintptr_t crosscon_hc_sg_id;

#define CROSSHYP_IMAGE_START     0x10000000UL
#define CROSSHYP_OFF          0x41UL
#define CROSSHYP_HC_ADDR         CROSSHYP_IMAGE_START+CROSSHYP_OFF

struct crosshyp_hypercall_result {
	uintptr_t r0;
	uintptr_t r1;
	uintptr_t r2;
};

/*
 * ARMv8-R hypercall ABI used by CROSSCON on the RZ/T2M:
 *
 *   r0 = hypercall/service ID
 *   r1 = first argument
 *   r2 = second argument
 *
 * HVC transfers control from the guest to the hypervisor.  This is different
 * from ARMv8-M, where the existing LPC55S69 port calls the NSC gateway at
 * 0x10000041 instead.
 */
#ifdef PLAT_RZT2M
static inline uintptr_t crosshyp_hypercall(uintptr_t arg0, uintptr_t arg1,
					    uintptr_t arg2)
{
	register uintptr_t r0 __asm__("r0") = arg0;
	register uintptr_t r1 __asm__("r1") = arg1;
	register uintptr_t r2 __asm__("r2") = arg2;

	__asm__ volatile (
		"hvc #0"
		: "+r" (r0), "+r" (r1), "+r" (r2)
		:
		: "cc", "memory");

	return r0;
}
#else
extern void (*crosshyp_hypercall)(unsigned int, unsigned int, unsigned int);
#endif
#ifdef PLAT_RZT2M
/*
 * The mTower VM receives the next request in r1/r2 when the hypercall
 * returns.  A normal C return value only exposes r0, so capture all three
 * registers in the same asm statement that performs the call.
 */
static inline struct crosshyp_hypercall_result
crosshyp_hypercall_with_result(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2)
{
	register uintptr_t r0 __asm__("r0") = arg0;
	register uintptr_t r1 __asm__("r1") = arg1;
	register uintptr_t r2 __asm__("r2") = arg2;
	struct crosshyp_hypercall_result result;

	__asm__ volatile (
		"hvc #0"
		: "+r" (r0), "+r" (r1), "+r" (r2)
		:
		: "cc", "memory");

	result.r0 = r0;
	result.r1 = r1;
	result.r2 = r2;
	return result;
}
#endif


#endif /* __INCLUDE_MTOWER_CROSSHYP_H */
