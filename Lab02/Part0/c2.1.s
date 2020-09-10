.text

.global start
    start:
        mov r0, #1
        mov r1, #2
        add r1, r1, r0
        ldr r1, [r2]
    stop:
        b stop
.data
