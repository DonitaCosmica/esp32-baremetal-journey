ENTRY(call_start)

MEMORY
{
  iram (RX) : ORIGIN = 0x40378000, LENGTH = 0x10000   /* 64 KB */
  dram (RW) : ORIGIN = 0x3FC98000, LENGTH = 0x30000   /* justo después, sin solapar */
}

_stack_top = ORIGIN(dram) + LENGTH(dram);

SECTIONS
{
  .iram : {
    *(.literal .literal.*)      /* literales ANTES del código (l32r solo mira hacia atrás) */
    *(.text.entry)
    *(.text .text.*)
  } > iram

  .dram : {
    *(.rodata .rodata.*)
    *(.data .data.*)
  } > dram

  .bss (NOLOAD) : {
    _bss_start = .;
    *(.bss .bss.*) *(COMMON)
    _bss_end = .;
  } > dram

  /DISCARD/ : { *(.comment) *(.xt.*) }
}