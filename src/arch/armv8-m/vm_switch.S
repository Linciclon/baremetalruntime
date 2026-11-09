/** Measure one CROSSCON VM transition with the shared Non-secure DWT. */

.syntax unified
.text
.thumb

.equ CROSSHYP_HC_ADDR, 0x10000041

.thumb_func
.align 2
.global measure_vm_context_switch
.type measure_vm_context_switch, %function
measure_vm_context_switch:
    push {r4, lr}
    mov r4, r0

    ldr r2, =0xE0001004
    movs r1, #0
    str r1, [r2]
    ldr r2, =0xE0001000
    ldr r1, [r2]
    orr r1, r1, #1
    str r1, [r2]
    dsb
    isb

    mov r0, r4
    movs r1, #0
    movs r2, #0
    ldr r3, =CROSSHYP_HC_ADDR
    blx r3

    /* This is the first guest instruction after the VM is resumed. */
    ldr r1, =0xE0001004
    ldr r0, [r1]
    ldr r1, =0xE0001000
    ldr r2, [r1]
    bic r2, r2, #1
    str r2, [r1]
    dsb
    isb
    pop {r4, pc}

.size measure_vm_context_switch, . - measure_vm_context_switch
