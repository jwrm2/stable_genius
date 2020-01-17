sysconf_DATA = $(srcdir)/bar.conf $(srcdir)/foo.conf

# The install target doesn't really meet our requirements. Instead, make the run
# target put everything into the file tree that will become the root filesystem.
# However, run is made PHONY by automake, so have another non-PHONY target to do
# the actual install. Just pick o0ne named file as the target. The target
# depends on all the files and installs them all.

run-local: $(sysconfdir)/bar.conf

$(sysconfdir)/bar.conf: $(sysconf_DATA)
	$(MKDIR_P) $(sysconfdir)
	$(INSTALL) -m 644 $(sysconf_DATA) $(sysconfdir)
