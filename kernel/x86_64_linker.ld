OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

PHDRS
{
    text     PT_LOAD;
    rodata   PT_LOAD;
    data     PT_LOAD;
}


SECTIONS
{
    . = 0xffffffff80000000;

    .text : {
        *(.text .text.*)
    } :text

    _etext = .; 

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .init_array : {
        __init_array = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP(*(.init_array .ctors))
        __init_array_end = .;
    } :rodata

    .fini_array : {
        __fini_array = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP(*(.fini_array .dtors))
        __fini_array_end = .;
    } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data :
    {
        _sdata = .;
        *(.data .data.*)
        _edata = .;
    } :data
    
    _sidata = LOADADDR(.data);

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .bss (NOLOAD) : {
        _sbss = .;
        *(.bss .bss.*)
        *(COMMON)
        _ebss = .;
    } :data

    . = ALIGN(8);
    .gcc_except_table : {
        *(.gcc_except_table)
    } :rodata

    .eh_frame_hdr : {
        *(.eh_frame_hdr)
    } :rodata

    .eh_frame : {
        *(.eh_frame)
    } :rodata
    
    /DISCARD/ : {
        *(.note .note.*)
        *(.comment)
    }
}