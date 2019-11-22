# The install target doesn't really meet our requirements. Instead, make the run
# target create our disk images.
run-local: @HDA@ @HDB@

# Create the first disk.
@HDA@: $(srcdir)/@HDA_PART1_FS@ $(srcdir)/@HDA_PART2_FS@ $(shell find $(prefix))
	$(MKDIR_P) @fusemntdir@
	@truncate@ -s @HDA_OFF@ $@
	@truncate@ -s @HDA_PART1_SZ@ $@.part1
	@mkfsext2@ $@.part1
	@fusemnt@ -o rw+ $@.part1 @fusemntdir@
	$(srcdir)/@HDA_PART1_FS@ @fusemntdir@ $(prefix)
	@fuseumnt@ @fusemntdir@
	cat $@.part1 >> $@
	rm $@.part1
	@truncate@ -s @HDA_PART2_SZ@ $@.part2
	@mkfsext2@ $@.part2
	@fusemnt@ -o rw+ $@.part2 @fusemntdir@
	$(srcdir)/@HDA_PART2_FS@ @fusemntdir@
	@fuseumnt@ @fusemntdir@
	cat $@.part2 >> $@
	rm $@.part2
	@parted@ $@ mklabel msdos -s
	@parted@ $@ mkpart primary ext2 1 8 -s
	@parted@ $@ mkpart primary ext2 8 12 -s
	rmdir @fusemntdir@

# Create the second disk.
@HDB@: $(srcdir)/@HDB_PART1_FS@ $(srcdir)/@HDB_PART2_FS@
	$(MKDIR_P) @fusemntdir@
	@truncate@ -s @HDB_OFF@ $@
	@truncate@ -s @HDB_PART1_SZ@ $@.part1
	@mkfsext2@ $@.part1
	@fusemnt@ -o rw+ $@.part1 @fusemntdir@
	$(srcdir)/@HDB_PART1_FS@ @fusemntdir@
	@fuseumnt@ @fusemntdir@
	cat $@.part1 >> $@
	rm $@.part1
	@truncate@ -s @HDB_PART2_SZ@ $@.part2
	@mkfsext2@ $@.part2
	@fusemnt@ -o rw+ $@.part2 @fusemntdir@
	$(srcdir)/@HDB_PART2_FS@ @fusemntdir@
	@fuseumnt@ @fusemntdir@
	cat $@.part2 >> $@
	rm $@.part2
	@parted@ $@ mklabel gpt -s
	@parted@ $@ mkpart first_gpt ext2 1 8 -s
	@parted@ $@ mkpart second_gpt ext2 8 12 -s
	rmdir $(fusemntdir)
