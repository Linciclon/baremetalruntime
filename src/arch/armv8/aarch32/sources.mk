arch_c_srcs:=
arch_s_srcs+=$(addprefix $(ARCH_SUB)/, exceptions.S start.S)

ifeq ($(MPU),)
arch_s_srcs+=$(addprefix $(ARCH_SUB)/, page_tables.S)
endif
