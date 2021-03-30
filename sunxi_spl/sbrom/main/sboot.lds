OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
	. = SBROMSWADDR;
	. = ALIGN(4);

	.head :
	{
		main/sbromsw_head.o (.rodata)
	}
        .hash :
        {
		main/sbromsw_hash.o  (.data)
        }
	.text :
	{
		main/brom.o (.text)
		*(.text)
	}
	. = ALIGN(16);
  	.rodata : { *(.rodata) }
	. = ALIGN(16);
  	.data : { *(.data) }

	. = ALIGN(4);
	.bss :
	{
	__bss_start = .;
		*(.bss)
	}
	. = ALIGN(4);
	__bss_end = .;

	_end = .;
}



