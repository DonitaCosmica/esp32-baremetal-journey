    .section .text.entry, "ax"
    .global  call_start
    .align   4
call_start:
    movi    a0, 0              /* sin dirección de retorno */
    movi    a1, _stack_top     /* stack pointer: lo ponemos nosotros */
    call4   app_main           /* llamada con ventana (ABI windowed) */
1:  j       1b