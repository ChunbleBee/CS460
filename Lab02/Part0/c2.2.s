.text
.global start
    start:
        ldr sp, = stack_top
        b1 sum
    stop:
        b stop
    sum:
        stmfd sp!, {r0-r4, lr}
        mov r0, #0
        ldr r1, = Array
        ldr r2, = N
        ldr r2, [r2]
    loop:
        ldr r3, [r1], #4
        add r0, r0, r3
        sub r2, r2, #1
        cmp r2, #0
        bne loop
        ldr r4, = Result
        str r0, [r4]
        ldmfd sp!, {r0-r4, pc}
.data
    N: .word 10
    Array: .word 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    Result: .word 0