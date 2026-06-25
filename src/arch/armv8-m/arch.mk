CROSS_COMPILE ?= arm-none-eabi-

ARM_PROFILE?=armv8-m.main
ARCH_GENERIC_FLAGS +=-march=$(ARM_PROFILE) $(ARCH_SUB_GENERIC_FLAGS) -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=soft
ARCH_ASFLAGS =
ARCH_CPPFLAGS =
ARCH_LDFLAGS =
