# User space subdirectory and kernel must be built after the standard library.
SUBDIRS = stdlib kernel user disks

# Extra steps to make the disks.
disks: @HDA@ @HDB@

@HDA@: disks/@HDA@
	@qemuimg@ convert -O qcow2 $< $@

@HDB@: disks/@HDB@
	@qemuimg@ convert -O qcow2 $< $@

# Clean the disk images.
diskclean:
	rm -f @HDA@ @HDB@ disks/@HDA@ disks/@HDB@

.PHONY: diskclean

# Run the emulator. Recurses through subdirectories first, to build the bootable
# iso and disk images.
run-local: disks
	@qemu@ @RUN_FLAGS@ @SERIAL_FLAGS@ @MEMORY_FLAGS@ @DISK_FLAGS@ @BOOT_FLAGS@

# Delete disk images and serial output.
clean-local:
	rm -f @HDA@ @HDB@ serial.out

