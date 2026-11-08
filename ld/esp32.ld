ENTRY(call_start)

MEMORY
{
  iram (RX) : ORIGIN = 0x40080000, LENGTH = 0x20000
  dram (RW) : ORIGIN = 0x3FFB0000, LENGTH = 0x20000
}

_stack_top = ORIGIN(dram) + LENGTH(dram);

SECTIONS
{
  .iram : {
    *(.literal .literal.*)
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

  /DISCARD/ : { *(.comment) }
}