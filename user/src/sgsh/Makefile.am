bin_PROGRAMS = sgsh

# List of headers and sources for sgsh.
sgsh_SOURCES = start.s

# Compiler flas for sgsh
sgsh_CPPFLAGS = @common_cppflags@ @user_cppflags@
sgsh_CXXFLAGS = @common_cxxflags@
sgsh_CCASFLAGS = @common_ccasflags@
sgsh_LDFLAGS = @common_ldflags@ @user_ldflags@
sgsh_LDADD = @user_libs@ @common_libs@

# The install target doesn't really meet our requirements. Instead, make the run
# target put everything into the file tree that will become the root filesystem.
# However, run is made PHONY by automake, so have another non-PHONY target to do
# the actual install.
run-local: $(bindir)/sgsh

$(bindir)/sgsh: $(bin_PROGRAMS)
	$(MKDIR_P) $(bindir)
	$(INSTALL) $(bin_PROGRAMS) $(bindir)

