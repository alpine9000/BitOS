OUTPUT_ARCH(sh)

MEMORY
{
/*  ram    (rwx) : ORIGIN = 0xE2FFFFFF, LENGTH = 0x1D000000*/
    ram    (rwx) : ORIGIN = 0xf7ffffff, LENGTH = 0x8000000
/*   ram    (rwx) : ORIGIN = 0xff7fffff, LENGTH = 0x800000*/
    rom    (rx)  : ORIGIN = 0x00000000, LENGTH = 0xFFFFFF
    peripheral (rw) : ORIGIN = 0x10000000, LENGTH = 0x29F7 
    videoRam (rw) : ORIGIN = 0x20000000, LENGTH = 0x10000000
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

         .data  : AT (_mdata)
       {
	  _data = .;
          . = ALIGN(4);
          *(.data)           /* .data sections */
          *(.data.*)
          _edata = .;
       } > ram

	.bss :
	{
		_bss = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		_ebss = .;
		_end = .;
	} > ram

      .heap  (NOLOAD) :
      {
        _heap = .;
      } > ram

      .stack 0xFFFFFFA0: AT (0xFFFFFFA0)
      {
	_stack = .;
      }

     .peripheral : 
     {
       _peripheral = .;
     } > peripheral

     .videoRam : 
     {
       _videoRam = .;
     } > videoRam


}