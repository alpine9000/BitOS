OUTPUT_ARCH(sh)

MEMORY
{
    ram    (rwx) : ORIGIN = 0xFFFF0000, LENGTH = 0xBFFF
    rom    (rx)  : ORIGIN = 0x00000000, LENGTH = 0xFFFFFF
    peripheral (rw) : ORIGIN = 0xFFFFD000, LENGTH = 0x29F7
}

SECTIONS
{
	.text : 
	{
          *(.vector)
          . = 0x1000;
          *(.text)
          *(.text.*)
	} > rom


	.rodata :
       	{
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
	} >rom

	.eh_frame_hdr :
	{
		*(.eh_frame_hdr)
	} >rom

	.eh_frame :
	{
		*(.eh_frame)
	} >rom

	.tors :
	{
		__CTOR_LIST__ = .;
		___ctors = .;
		*(.ctors)
		___ctors_end = .;
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		___dtors = .;
		*(.dtors)
		___dtors_end = .;
	     __DTOR_END__ = .;
           _mdata = .;
       } > rom

       .data 0xFFFF6000 : AT (_mdata)
/*       .data 0xFFFF6000 :*/
       {
	  _data = .;
          . = ALIGN(4);
          *(.data)           /* .data sections */
          *(.data.*)           /* .data sections */
          _edata = .;
       }

	.bss :
	{
		_bss = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		_ebss = .;
		_end = .;
	} > ram

       .heap :
       {
		*(.heap)
		*(.heap.*)
       } > ram

/*      .bss : 
      {
	 _bss = .;
        . = 0xBAF0;
	_end = .;
	_ebss = .;
      }  > ram*/

      .stack 0xFFFFBFA0: AT (0xFFFFBFA0)
      {
	_stack = .;
      }

     .peripheral : 
     {
       __peripheral = .;
     } > peripheral


}