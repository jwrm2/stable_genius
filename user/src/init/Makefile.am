bin_PROGRAMS = init

# List of headers and sources for init.
init_SOURCES = main.cpp

# Compiler flas for init
init_CPPFLAGS = @common_cppflags@ @user_cppflags@
init_CXXFLAGS = @common_cxxflags@
init_CCASFLAGS = @common_ccasflags@
init_LDFLAGS = @common_ldflags@ @user_ldflags@
init_LDADD = @user_libs@ @common_libs@
EXTRA_init_DEPENDENCIES = @user_dependencies@

# The install target doesn't really meet our requirements. Instead, make the run
# target put everything into the file tree that will become the root filesystem.
# However, run is made PHONY by automake, so have another non-PHONY target to do
# the actual install, which should be a real file.
run-local: $(bindir)/init

$(bindir)/init: $(bin_PROGRAMS)
	$(MKDIR_P) $(bindir)
	$(INSTALL) $(bin_PROGRAMS) $(bindir)

