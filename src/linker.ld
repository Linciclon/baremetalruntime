/** 
 * Bao, a Lightweight Static Partitioning Hypervisor 
 *
 * Copyright (c) Bao Project (www.bao-project.org), 2019-
 *
 * Authors:
 *      Jose Martins <jose.martins@bao-project.org>
 *      Sandro Pinto <sandro.pinto@bao-project.org>
 *
 * Bao is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation, with a special exception exempting guest code from such
 * license. See the COPYING file in the top-level directory for details. 
 *
 */

#include <core.h>
#include <plat.h>

MEMORY {
#if defined(PLAT_NON_UNIFIED_MEM)
    ROM (rx)     : ORIGIN = PLAT_CODE_MEM_BASE, LENGTH = PLAT_CODE_MEM_SIZE
#endif
    RAM (rwx)    : ORIGIN = MEM_BASE, LENGTH = MEM_SIZE
}

ENTRY(_start)

SECTIONS {

    .start : {
        *(.start)
    }

    .text :  {
        *(.text*)
    }

    .rodata :  {
        *(.rodata*)
    }

	. = ALIGN(32); /* start RW sections in separate page */
	_data_lma_start = .;

#if defined(PLAT_NON_UNIFIED_MEM)
    . = PLAT_MEM_BASE;
    _data_vma_start = .;
#endif

    .data : AT(_data_lma_start){
        *(.data .data.*)
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata .sdata.* .sdata2.*)
    }

#if defined(PLAT_NON_UNIFIED_MEM)
    _data_lma_end = _data_lma_start + SIZEOF(.data);
#endif

    .latency (NOLOAD) :  {
        KEEP(*(.latency))
    }

    .bss (NOLOAD) :  {
        __bss_start = .;
        *(.bss* .sbss*)
        *(COMMON)
        __bss_end = .;
    }

    . = ALIGN(16);
    PROVIDE(_stack_base = .);
#if defined(PLAT_NON_UNIFIED_MEM)
    . = . + STACK_SIZE;
    PROVIDE(_stack_top = .);
#else
    . = . + 2M; /* 2M of total stack size */
#endif
    PROVIDE(_heap_base = .);
}
