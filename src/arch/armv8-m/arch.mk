CROSS_COMPILE ?= arm-none-eabi-

ARM_CPU?=cortex-m33
ARCH_GENERIC_FLAGS += -mcpu=$(ARM_CPU) $(ARCH_SUB_GENERIC_FLAGS) -mthumb -mcmse -mfpu=fpv5-sp-d16 -mfloat-abi=soft
ARCH_ASFLAGS =
ARCH_CPPFLAGS =
ARCH_LDFLAGS =
