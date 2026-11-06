ARCH:=armv8
ARM_PROFILE:=armv8-r
ARM_FLOAT_ABI:=soft
GIC_VERSION:=GICV3
drivers:=rzt2m_uart rzt2m_timer
MPU:=y
ARCH_SUB:=aarch32
SINGLE_CORE:=y

# Keep the channels used by the known-good bare-metal implementation.  Both
# TCIV4 (INTID 125) and TCIV7 (INTID 138) are forwarded by the hypervisor VM.
MTU_CHANNEL?=4
ifeq ($(filter $(MTU_CHANNEL),4 7),)
$(error Unsupported MTU_CHANNEL=$(MTU_CHANNEL); supported channels are 4 and 7)
endif
CPPFLAGS+=-DMTU_CHANNEL=$(MTU_CHANNEL)
