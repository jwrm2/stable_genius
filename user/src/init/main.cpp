// C linkage, so we can call from assembly
extern "C" {

// .data entry
unsigned int deadbeef_data = 0xDEADBEEF;

// .rodata entry
extern const unsigned int deadbabe_rodata = 0xDEADBABE;

// .bss entry
unsigned int cafebabe_bss;

int cxx_main()
{
    cafebabe_bss = 0xCAFEBABE;
    return cafebabe_bss;
}

// End extern "C"
}
