# Create the kernel.
noinst_PROGRAMS = stable_genius
grubcfg = grub.cfg
noinst_DATA = $(grubcfg)

kernel_includes = @kernel_include_dir@/AOut.h @kernel_include_dir@/Elf.h @kernel_include_dir@/Gdt.h @kernel_include_dir@/InterruptHandler.h \
    @kernel_include_dir@/Keyboard.h @kernel_include_dir@/PageDescriptorTable.h @kernel_include_dir@/Pic.h @kernel_include_dir@/Ps2Controller.h \
    @kernel_include_dir@/SignalManager.h @kernel_include_dir@/Vbe.h @kernel_include_dir@/DevFileSystem.h @kernel_include_dir@/Ext.h @kernel_include_dir@/Ide.h \
    @kernel_include_dir@/io.h @kernel_include_dir@/Logger.h @kernel_include_dir@/PageFrameAllocator.h @kernel_include_dir@/Pit.h @kernel_include_dir@/RttiTest.h \
    @kernel_include_dir@/Syscall.h @kernel_include_dir@/VgaController.h @kernel_include_dir@/Device.h @kernel_include_dir@/File.h @kernel_include_dir@/Idt.h \
    @kernel_include_dir@/Kernel.h @kernel_include_dir@/MultiBoot.h @kernel_include_dir@/paging.h @kernel_include_dir@/Process.h @kernel_include_dir@/Scheduler.h \
    @kernel_include_dir@/Tty.h @kernel_include_dir@/VgaCursor.h @kernel_include_dir@/DiskPartition.h @kernel_include_dir@/FileSystem.h \
    @kernel_include_dir@/interrupt.h @kernel_include_dir@/KernelHeap.h @kernel_include_dir@/no_heap_util.h @kernel_include_dir@/Pci.h \
    @kernel_include_dir@/ProcTable.h @kernel_include_dir@/Serial.h @kernel_include_dir@/util.h @kernel_include_dir@/VgaIo.h
kernel_cpp_sources = @kernel_cpp_dir@/AOut.cpp @kernel_cpp_dir@/Ext.cpp @kernel_cpp_dir@/Idt.cpp @kernel_cpp_dir@/Keyboard.cpp @kernel_cpp_dir@/PageDescriptorTable.cpp \
    @kernel_cpp_dir@/Process.cpp @kernel_cpp_dir@/Serial.cpp @kernel_cpp_dir@/Vbe.cpp @kernel_cpp_dir@/DevFileSystem.cpp @kernel_cpp_dir@/File.cpp \
    @kernel_cpp_dir@/InterruptHandler.cpp @kernel_cpp_dir@/klib_cstdlib_impl.cpp @kernel_cpp_dir@/PageFrameAllocator.cpp @kernel_cpp_dir@/ProcTable.cpp \
    @kernel_cpp_dir@/SignalManager.cpp @kernel_cpp_dir@/VgaController.cpp @kernel_cpp_dir@/Device.cpp @kernel_cpp_dir@/FileSystem.cpp @kernel_cpp_dir@/Kernel.cpp \
    @kernel_cpp_dir@/Logger.cpp @kernel_cpp_dir@/Pci.cpp @kernel_cpp_dir@/Ps2Controller.cpp @kernel_cpp_dir@/Syscall.cpp @kernel_cpp_dir@/VgaCursor.cpp \
    @kernel_cpp_dir@/DiskPartition.cpp @kernel_cpp_dir@/Gdt.cpp @kernel_cpp_dir@/KernelHeap.cpp @kernel_cpp_dir@/MultiBoot.cpp @kernel_cpp_dir@/Pic.cpp \
    @kernel_cpp_dir@/RttiTest.cpp @kernel_cpp_dir@/Tty.cpp @kernel_cpp_dir@/VgaIo.cpp @kernel_cpp_dir@/Elf.cpp @kernel_cpp_dir@/Ide.cpp \
    @kernel_cpp_dir@/kernel_main.cpp @kernel_cpp_dir@/no_heap_util.cpp @kernel_cpp_dir@/Pit.cpp @kernel_cpp_dir@/Scheduler.cpp @kernel_cpp_dir@/util.cpp
kernel_asm_sources = @kernel_asm_dir@/interrupt.s @kernel_asm_dir@/io.s @kernel_asm_dir@/launch_process.s @kernel_asm_dir@/loader.s @kernel_asm_dir@/load_gdt.s \
    @kernel_asm_dir@/paging.s @kernel_asm_dir@/yield.s
kernel_linker_sources = @kernel_dir@/link.ld

# List of headers and sources for the kernel.
stable_genius_SOURCES = $(kernel_includes) $(kernel_cpp_sources) $(kernel_asm_sources) $(kernel_linker_sources)

# Compiler flas for the kernel
stable_genius_CPPFLAGS = @common_cppflags@ @kernel_cppflags@ @kernel_defs@
stable_genius_CXXFLAGS = @common_cxxflags@
stable_genius_CCASFLAGS = @common_ccasflags@
stable_genius_LDFLAGS = @common_ldflags@ @kernel_ldflags@
stable_genius_LDADD = @kernel_libs@ @common_libs@

# Makes the bootable ISO
@iso_name@: @kernel@ $(srcdir)/$(grubcfg)
	$(MKDIR_P) iso iso/boot/grub
	$(INSTALL) @kernel@ iso
	$(SED) "s/ISO_NAME/@kernel@/g" $(srcdir)/$(grubcfg) > iso/boot/grub/$(grubcfg)
	@grubmkrescue@ -o @iso_name@ -d @grubcore@ iso

run-local: @iso_name@



