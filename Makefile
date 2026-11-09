# SPDX-License-Identifier: GPL-2.0
# Copyright (c) Bao Project and Contributors. All rights reserved.

# Prepare enviroment for baremetal-runtime
NAME:=baremetal
ROOT_DIR:=$(realpath .)
BUILD_DIR:=$(ROOT_DIR)/build/$(PLATFORM)

# To evaluate Secure Interrupt Latency 
#SECURE_INTERRUPT_LATENCY:=y

# To evaluate Non-Secure Interrupt Latency
#NONSECURE_INTERRUPT_LATENCY:=y

# To evaluate Interrupt Latency in Secure Context
#EXECUTION_IN_SECURE_CONTEXT:=y

# To evaluate Interrupt Latency in Non-Secure Context
#EXECUTION_IN_NONSECURE_CONTEXT:=y

# To run this inside a VM
RUN_INSIDE_VM:=y

# To evaluate context switch latency
CONTEXT_SWITCH_LATENCY:=y

# Use the platform timer instead of SysTick/the architectural timer.  This
# selects CTIMER0 on LPC55S69 and MTU channel 4/7 on RZ/T2M.
ifneq ($(CONTEXT_SWITCH_LATENCY),y)
ifneq ($(filter $(PLATFORM),lpc55s69 rzt2m),)
USINGTIMER:=y
endif
endif

# Setup baremetal-runtime build
include $(ROOT_DIR)/setup.mk

app_src_dir:=$(ROOT_DIR)/src
include $(app_src_dir)/sources.mk
C_SRC+=$(addprefix $(app_src_dir)/, $(src_c_srcs))

# Include the final baremetal-runtime build
include $(ROOT_DIR)/build.mk
