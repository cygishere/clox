AC_PREREQ([2.72])
AC_INIT([clox], [0.1], [https://github.com/cygishere/clox/issues], [clox], [https://github.com/cygishere/clox])

AC_CONFIG_SRCDIR([src/lox.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_INIT_AUTOMAKE([subdir-objects]) # https://autotools.info/automake/nonrecursive.html

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
