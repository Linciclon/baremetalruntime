core_c_srcs:=irq.c retarget.c

# The VirtIO console exists only when the image is hosted by a hypervisor.
ifneq ($(RUN_INSIDE_VM),)
core_c_srcs+=virtio_console.c
endif
