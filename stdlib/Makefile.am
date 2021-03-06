# Create both the kernel and user space libraries. The kernel library is only
# for linking the kernel and does not need to be 'installed'. We create a
# libstdc++ for user space. However, currently the C and C++ libraries are not
# seperate, so it's empty and everything is in libc. If I want true C user space
# programmes, I'll have to fix that. We also create libm for user space, since
# the g++ linker line wants it, but it's also currently empty.
lib_LIBRARIES = libc.a libstdcxx.a libm.a
crtdir = $(libdir)
crt_DATA = crti.o crtn.o crt0.o
noinst_LIBRARIES = libk.a

stdlib_includes_common = @stdlib_include_dir@/algorithm @stdlib_include_dir@/cctype @stdlib_include_dir@/cmath @stdlib_include_dir@/cstdlib @stdlib_include_dir@/cwchar \
    @stdlib_include_dir@/exception @stdlib_include_dir@/functional @stdlib_include_dir@/ios @stdlib_include_dir@/iterator @stdlib_include_dir@/map \
    @stdlib_include_dir@/new @stdlib_include_dir@/rbtree @stdlib_include_dir@/stdexcept @stdlib_include_dir@/string @stdlib_include_dir@/typeinfo \
    @stdlib_include_dir@/utility @stdlib_include_dir@/array @stdlib_include_dir@/cerrno @stdlib_include_dir@/cstdio @stdlib_include_dir@/cstring \
    @stdlib_include_dir@/cxxabi @stdlib_include_dir@/fstream @stdlib_include_dir@/initializer_list @stdlib_include_dir@/istream @stdlib_include_dir@/limits \
    @stdlib_include_dir@/memory @stdlib_include_dir@/ostream @stdlib_include_dir@/sstream @stdlib_include_dir@/streambuf @stdlib_include_dir@/system_error \
    @stdlib_include_dir@/type_traits @stdlib_include_dir@/vector
stdlib_includes_konly =
stdlib_includes_conly = @stdlib_include_dir@/fcntl.h @stdlib_include_dir@/initialise.h @stdlib_include_dir@/iostream @stdlib_include_dir@/unistd.h \
    @stdlib_include_dir@/UserHeap.h
stdlib_sources_common = @stdlib_cpp_dir@/cctype.cpp @stdlib_cpp_dir@/cstdio.cpp @stdlib_cpp_dir@/cstring.cpp @stdlib_cpp_dir@/cxxabi.cpp @stdlib_cpp_dir@/ios.cpp \
    @stdlib_cpp_dir@/new.cpp @stdlib_cpp_dir@/stdexcept.cpp @stdlib_cpp_dir@/system_error.cpp @stdlib_cpp_dir@/cmath.cpp @stdlib_cpp_dir@/cstdlib.cpp \
    @stdlib_cpp_dir@/cwchar.cpp @stdlib_cpp_dir@/exception.cpp @stdlib_cpp_dir@/istream.cpp @stdlib_cpp_dir@/ostream.cpp @stdlib_cpp_dir@/string.cpp \
    @stdlib_cpp_dir@/typeinfo.cpp @stdlib_cpp_dir@/UserHeap.cpp @stdlib_cpp_dir@/initialise.cpp
stdlib_sources_konly =
stdlib_sources_conly = @stdlib_asm_dir@/syscalls.s @stdlib_cpp_dir@/iostream.cpp

# List of headers and sources for the user space library.
libc_a_SOURCES = $(stdlib_includes_common) $(stdlib_includes_conly) $(stdlib_sources_common) $(stdlib_sources_conly)
include_HEADERS = $(stdlib_includes_common) $(stdlib_includes_conly)
crti.o: @stdlib_crt_dir@/crti.s
	$(CCAS) -c @common_ccasflags@ -o $@ $^
crtn.o: @stdlib_crt_dir@/crtn.s
	$(CCAS) -c @common_ccasflags@ -o $@ $^
crt0.o: @stdlib_crt_dir@/crt0.s
	$(CCAS) -c @common_ccasflags@ -o $@ $^
libstdcxx_a_SOURCES = @stdlib_cpp_dir@/empty.cpp
libm_a_SOURCES = @stdlib_cpp_dir@/empty.cpp

# Compiler flas for the user space library
libc_a_CPPFLAGS = @common_cppflags@ @stdlib_cppflags@
libc_a_CCASFLAGS = @common_ccasflags@
libc_a_CXXFLAGS = @common_cxxflags@ @stdlib_cxxflags@
libstdcxx_a_CPPFLAGS = @common_cppflags@ @stdlib_cppflags@
libstdcxx_a_CCASFLAGS = @common_ccasflags@
libstdcxx_a_CXXFLAGS = @common_cxxflags@ @stdlib_cxxflags@
libm_a_CPPFLAGS = @common_cppflags@ @stdlib_cppflags@
libm_a_CCASFLAGS = @common_ccasflags@
libm_a_CXXFLAGS = @common_cxxflags@ @stdlib_cxxflags@

# List of headers and sources for the kernel library.
libk_a_SOURCES = $(stdlib_includes_common) $(stdlib_includes_konly) $(stdlib_sources_common) $(stdlib_sources_konly)

# Compiler flas for the kernel library
libk_a_CPPFLAGS = @common_cppflags@ @kernel_cppflags@ @kernel_defs@
libk_a_CCASFLAGS = @common_ccasflags@
libk_a_CXXFLAGS = @common_cxxflags@ @stdlib_cxxflags@

# The user space stdlib, start files and includes need to be in place in the
# image directory so the user space builds can use that as the sysroot. Add a
# custom local command to the build to achieve this.
run-local: all-local

all-local: $(libdir)/libc.a

$(libdir)/libc.a: $(lib_LIBRARIES) $(include_HEADERS) $(crt_DATA)
	$(MKDIR_P) $(libdir)
	$(INSTALL) $(lib_LIBRARIES) $(libdir)
	mv $(libdir)/libstdcxx.a $(libdir)/libstdc++.a
	$(INSTALL) $(crt_DATA) $(crtdir)
	$(MKDIR_P) $(includedir)
	$(INSTALL) $(include_HEADERS) $(includedir)

