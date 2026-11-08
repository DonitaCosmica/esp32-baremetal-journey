    .section .text.entry, "ax"
    .global  call_start
    .align   4
call_start:
    movi    a0, 0
    movi    a1, _stack_top
    call4   app_main
1:  j       1b