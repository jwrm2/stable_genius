# Compulsory initialisation
AC_INIT([Stable Genius OS], [0.1])

# Ensure recent version of autoconf
AC_PREREQ([2.67])

# Initialise Automake. Keep built objects in their corrct subdirectories. Do not
# cause fatal errors with autoreconf for not having NEWS, AUTHORS etc.
AM_INIT_AUTOMAKE([subdir-objects foreign])
AM_SILENT_RULES([yes])

# This should not be installed on the system. Instead, we use installation
# process to create ISOs and hard disk images. Therefore, the build directory is
# the appropriate default installation directory.
AC_PREFIX_DEFAULT(["${PWD}/image"])

# Set the locations for the various sub projects
kernel_dir=kernel
stdlib_dir=stdlib
AC_SUBST(stdlib_dir, ["${stdlib_dir}"])
AC_SUBST(stdlib_include_dir, ["\$(srcdir)/include"])
AC_SUBST(stdlib_asm_dir, ["asm"])
AC_SUBST(stdlib_cpp_dir, ["cpp"])
AC_SUBST(stdlib_crt_dir, ["crt"])
AC_SUBST(kernel_dir, ["${kernel_dir}"])
AC_SUBST(kernel_include_dir, ["include"])
AC_SUBST(kernel_cpp_dir, ["cpp"])
AC_SUBST(kernel_asm_dir, ["asm"])

# Check that the kernel and stdlib source files exist.
AC_CONFIG_SRCDIR([kernel/cpp/Kernel.cpp])
AC_CONFIG_SRCDIR([kernel/asm/loader.s])
AC_CONFIG_SRCDIR([stdlib/cpp/cstdio.cpp])

# Ask for the creation of Makefiles from the templates.
AC_CONFIG_FILES([Makefile kernel/Makefile stdlib/Makefile user/Makefile user/etc/Makefile user/src/Makefile user/src/init/Makefile user/src/sgsh/Makefile disks/Makefile])

# Check for the GRUB tools.
AC_ARG_WITH([grubdir], [AS_HELP_STRING([--with-grubdir=DIR], [location of the GRUB tools for making the bootable image])], [grubdir=$(readlink -f "${withval}")], [grubdir="not found"])
AC_ARG_WITH([grubmkrescue], [AS_HELP_STRING([--with-grubmkrescue=EXC], [location of the grub-mkrescue executable])], [grubmkrescue=$(readlink -f "${withval}")], [grubmkrescue="not found"])
AC_ARG_WITH([grubcore], [AS_HELP_STRING([--with-grubcore=DIR], [location of the GRUB core installation files])], [grubcore=$(readlink -f "${withval}")], [grubcore="not found"])

# If grub-mkrescue was not specified, try to find it from grubdir, if it exists,
# or in the PATH if it doesn't.
if [[ "${grubmkrescue}" == "not found" ]]
then
    if [[ ! "${grubdir}" == "not found" -a -f "${grubdir}/grub-mkrescue" ]]
    then
        grubmkrescue="${grubdir}/grub-mkrescue"
    else
        autocheck="true"
        AC_PATH_PROG(grubmkrescue, grub-mkrescue, [not found])
    fi
fi
if [[ ! "x${autocheck}" = "xtrue" ]]
then
    AC_MSG_CHECKING([for grub-mkrescue])
    AC_MSG_RESULT([${grubmkrescue}])
fi
# Error messages if we've failed.
if [[ ! "${grubmkrescue}" == "not found" -a ! -f "${grubmkrescue}" ]]
then
    AC_MSG_ERROR([specified grub-mkrescue does not exist])
fi
if [[ "${grubmkrescue}" == "not found" ]]
then
    if [[ "${grubdir}" == "not found" ]]
    then
        AC_MSG_ERROR([could not find grub-mkrescue, specify location with --with-grubmkrescue=EXC])
    else
        AC_MSG_ERROR([could not find grub-mkrescue in specified grubdir])
    fi
fi

AC_MSG_CHECKING([for grub-core installation files])
# If grub-core was not specified, try to find it from grubdir, if it exists, or
# from grub-mkrescue if it doesn't.
if [[ "${grubcore}" == "not found" ]]
then
    if [[ ! "${grubdir}" == "not found" -a -d "${grubdir}/grub-core" ]]
    then
        grubcore="${grubdir}/grub-core"
    elif [[ ! "${grubmkrescue}" == "not-sepcified" -a -d "$(dirname "${grubmkrescue}")/grub-core" ]]
    then
        grubcore="$(dirname "${grubmkrescue}")/grub-core"
    fi
fi
AC_MSG_RESULT([${grubcore}])
# Error messages if we've failed.
if [[ ! "${grubcore}" == "not found" -a ! -d "${grubcore}" ]]
then
    AC_MSG_ERROR([specified grub-core does not exist])
fi
if [[ "${grubcore}" == "not found" ]]
then
    if [[ "${grubdir}" == "not found" ]]
    then
        AC_MSG_ERROR([could not find grub-core, specify location with --with-grubcore=DIR])
    else
        AC_MSG_ERROR([could not find grub-core in specified grubdir])
    fi
fi

# Substitute the variables.
AC_SUBST(grubmkrescue, ["${grubmkrescue}"])
AC_SUBST(grubcore, ["${grubcore}"])

# The kernel can only be compiled with i686-elf as the host system. Ensure we
# use that as the host.
case $host in 
    "i686-elf" | "i686-*-elf") ;;
    "")
    AC_MSG_ERROR([invalid value for \$host: none provided, but only i686-elf is suppoted]) ;;
    *)
    AC_MSG_ERROR([invalid value for \$host: $host provided, but only i686-elf is suppoted]) ;;
esac

# Ensure the build system has also been correctly set
AC_CANONICAL_BUILD

# Set the bootable iso name.
kernel="stable_genius"
AC_SUBST(kernel, ["${kernel}"])
iso="stable-genius-boot-${host}.iso"
AC_SUBST(iso_name, ["${iso}"])

# Check for the C++ compiler. We need the default flags to be freestanding, or
# the test compilation will fail.
user_cxxflags=${CXXFLAGS}
: ${CXXFLAGS="-ffreestanding -nostdlib"}
AC_PROG_CXX
CXXFLAGS=${user_cxxflags}

# Check for the C compiler. We need the default flags to be freestanding, or
# the test compilation will fail.
user_cflags=${CFLAGS}
: ${CFLAGS="-ffreestanding -nostdlib"}
AC_PROG_CC
# Check for required compiler generated standard headers
AC_CHECK_HEADERS([stddef.h], [AC_DEFINE([HAVE_STDDEF_H], 1, [Define to 1 if stddef.h is present])], [AC_MSG_ERROR(stddef.h not present)], [ ])
AC_CHECK_HEADERS([stdarg.h], [AC_DEFINE([HAVE_STDARG_H], 1, [Define to 1 if stdarg.h is present])], [AC_MSG_ERROR(stdarg.h not present)], [ ])
AC_CHECK_HEADERS([stdint.h], [AC_DEFINE([HAVE_STDINT_H], 1, [Define to 1 if stdint.h is present])], [AC_MSG_ERROR(stdint.h not present)], [ ])
AC_CHECK_HEADERS([unwind.h], [AC_DEFINE([HAVE_UNWIND_H], 1, [Define to 1 if unwind.h is present])], [AC_MSG_ERROR(unwind.h not present)], [ ])
AC_CHECK_HEADERS([float.h], [AC_DEFINE([HAVE_FLOAT_H], 1, [Define to 1 if float.h is present])], [AC_MSG_ERROR(float.h not present)], [ ])
AC_CHECK_HEADERS([limits.h], [AC_DEFINE([HAVE_LIMITS_H], 1, [Define to 1 if limits.h is present])], [AC_MSG_ERROR(limits.h not present)], [ ])
CFLAGS=${user_cflags}

# Common CXXFLAGS and specifics for components.
AC_SUBST(common_cxxflags, ["-std=c++11 -ffreestanding -O2 -Wall -Wextra -Werror"])
AC_SUBST(stdlib_cxxflags, ["-fPIC"])

# Common CCASFLAGS and specifics for components.
AC_SUBST(common_ccasflags, ["-Wa,--fatal-warnings"])

# Common CPPFLAGS and specifics for components.
AC_SUBST(common_cppflags, [""])
AC_SUBST(user_cppflags, ["-I\$(top_srcdir)/${stdlib_dir}/include"])
AC_SUBST(kernel_cppflags, ["-I\$(top_srcdir)/${stdlib_dir}/include -I\$(top_srcdir)/${kernel_dir}/include"])
AC_SUBST(kernel_defs, ["-DNMSP=klib"])

# Common LDFLAGS and specifics for components.
AC_SUBST(common_ldflags, ["-O2 -nostdlib -L\$(top_builddir)/${stdlib_dir}"])
AC_SUBST(common_libs, ["-lgcc"])
AC_SUBST(kernel_ldflags, ["-T \$(top_srcdir)/${kernel_dir}/link.ld"])
AC_SUBST(kernel_libs, ["-lk"])
AC_SUBST(user_ldflags, [""])
AC_SUBST(user_libs, ["-lc"])

# Start files necessary for user space linking. crtbegin.o and crtend.o are
# provided by the compiler, which can tell us where they are.
CRTBEGIN_OBJ=$($CXX $CXXFLAGS -print-file-name=crtbegin.o)
CRTEND_OBJ=$($CXX $CXXFLAGS -print-file-name=crtbegin.o)
AC_SUBST(crtbegin, [${CRTBEGIN_OBJ}])
AC_SUBST(crtend, [${CRTEND_OBJ}])

# Check for other necessary compilation programs.
AC_PROG_RANLIB
AC_PROG_SED
AM_PROG_AS

# Check for the archiver, then override ARFLAGS to get rid of the 'u' option.
AM_PROG_AR
AC_SUBST(ARFLAGS, ["cr"])

# The install target doesn't really meet our needs for making the disk images.
# Use a recursive run target instead. This is called 'run-local' in the automake
# files.
AM_EXTRA_RECURSIVE_TARGETS([run])

# Flags to do with making the disk images.
AC_SUBST(fusemntdir, ["mnt"])
HDA="hda.img"
AC_SUBST(HDA, ["${HDA}"])
AC_SUBST(HDA_PART1_FS, ["make_sda1.sh"])
AC_SUBST(HDA_PART2_FS, ["make_sda2.sh"])
AC_SUBST(HDA_OFF, ["1048576"])
AC_SUBST(HDA_PART1_SZ, ["7340032"])
AC_SUBST(HDA_PART2_SZ, ["4194304"])
HDB="hdb.img"
AC_SUBST(HDB, ["${HDB}"])
AC_SUBST(HDB_PART1_FS, ["make_sdb1.sh"])
AC_SUBST(HDB_PART2_FS, ["make_sdb2.sh"])
AC_SUBST(HDB_OFF, ["1048576"])
AC_SUBST(HDB_PART1_SZ, ["7340032"])
AC_SUBST(HDB_PART2_SZ, ["4194304"])

# Programs for making the disk images.
AC_PATH_PROG(truncate, truncate, [not found])
if [[ "${truncate}" == "not found" ]]
then
    AC_MSG_ERROR([could not find truncate])
fi
AC_SUBST(truncate, ["${truncate}"])

AC_PATH_PROG(mkfsext2, mkfs.ext2, [not found])
if [[ "${mkfsext2}" == "not found" ]]
then
    AC_MSG_ERROR([could not find mkfs.ext2])
fi
AC_SUBST(mkfsext2, ["${mkfsext2}"])

AC_PATH_PROG(fusemnt, fuse-ext2, [not found])
if [[ "${fusemnt}" == "not found" ]]
then
    AC_MSG_ERROR([could not find fuse-ext2])
fi
AC_SUBST(fusemnt, ["${fusemnt}"])

AC_PATH_PROG(fuseumnt, fusermount, [not found])
if [[ "${fuseumnt}" == "not found" ]]
then
    AC_MSG_ERROR([could not find fusermount])
fi
AC_SUBST(fuseumnt, ["${fuseumnt} -u"])

AC_PATH_PROG(parted, parted, [not found])
if [[ "${parted}" == "not found" ]]
then
    AC_MSG_ERROR([could not find parted])
fi
AC_SUBST(parted, ["${parted}"])

AC_PATH_PROG(qemuimg, qemu-img, [not found])
if [[ "${qemuimg}" == "not found" ]]
then
    AC_MSG_ERROR([could not find qemu-img])
fi
AC_SUBST(qemuimg, ["${qemuimg}"])

AC_PATH_PROG(xorriso, xorriso, [not found])
if [[ "${xorriso}" == "not found" ]]
then
    AC_MSG_ERROR([could not find xorriso])
fi

# Flags to do with running the OS.
AC_SUBST(RUN_FLAGS, ["-monitor stdio"])
SERIAL="serial.out"
AC_SUBST(HDB, ["${HDB}"])
AC_SUBST(SERIAL_FLAGS, ["-serial file:${SERIAL}"])
AC_SUBST(MEMORY_FLAGS, ["-m 1G"])
AC_SUBST(DISK_FLAGS, ["-hda ${HDA} -hdb ${HDB} -cdrom kernel/${iso}"])
AC_SUBST(BOOT_FLAGS, ["-boot d"])

# Programs for running the OS.
AC_PATH_PROG(qemu, qemu-system-i386, [not found])
if [[ "${qemu}" == "not found" ]]
then
    AC_MSG_ERROR([could not find qemu-system-i386])
fi
AC_SUBST(qemu, ["${qemu}"])

# Generate output
AC_OUTPUT

