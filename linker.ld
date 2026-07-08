ENTRY(_start)

SECTIONS
{
  /* needed so that global variables can work correctly (and aparently also string literals)*/
  . = 0x1000;

  .text : {*(.text*)}
  .bss    : { *(.bss*) *(COMMON) }
}
