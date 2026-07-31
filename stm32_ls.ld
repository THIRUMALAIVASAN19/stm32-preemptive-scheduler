ENTRY(Reset_Handler)

MEMORY
{
	FLASH(rx):ORIGIN =0x08000000,LENGTH =1024K
	SRAM(rwx):ORIGIN =0x20000000,LENGTH =128K
}



_max_heap_size = 0x400; /* A symbol declaration Not a variable !!! */
_max_stack_size = 0x200; /* A symbol declaration. Not a variable !!!*/

SECTIONS
{
	.text :
	 {
	   *(.isr_vector)
	   *(.text)
	   *(.text.*)
	   *(.init*)
	   *(.fini)
	   *(.rodata)
	   *(.rodata.*)
	   . = ALIGN(4);
	   _etext = .; /* Store the updated location counter value in to a symbol 'end_of_test' */
	 }> FLASH
        _la_data = LOADADDR(.data);
	.data :
	{
	   _sdata = .; /* Assign a value to a symbol 'start_of_data' */
	   *(.data)
	   *(.data.*)
	   . = ALIGN(4);
	   _edata = .;
	}> SRAM AT> FLASH
	
	.bss :
	{
	   _sbss = .;
	   __bss_start__ = _sbss;
	   *(.bss)
	    *(.bss.*)
 	   *(COMMON)
	   . = ALIGN(4);
	   _ebss = .;
	   __bss_end__ = _ebss;
	   . = ALIGN(4);
	   end = .;
	   __end__ =.;
	}> SRAM
}

