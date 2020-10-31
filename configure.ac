AC_INIT([CROBOTS], [1.2-beta1],, [crobots], [https://github.com/troglobit/crobots/])
AC_CONFIG_AUX_DIR(aux)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 foreign])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_FILES([Makefile doc/Makefile examples/Makefile src/Makefile])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_YACC
AM_PROG_LEX

AC_HEADER_STDC

# Check for a common math function in -lm
AC_SEARCH_LIBS([cos], [m])

AX_WITH_CURSES
AS_IF([test "x$ax_cv_curses" != "xyes" ], [AC_MSG_ERROR([curses library not found])])

AC_OUTPUT

DOCDIR=`eval echo $docdir`
DOCDIR=`eval echo $DOCDIR`
cat <<EOF

------------------ Summary ------------------
 $PACKAGE_NAME version $PACKAGE_VERSION
  Install prefix........: $prefix
  Doc & examples prefix.: $DOCDIR
  Curses CFLAGS.........: $CURSES_CFLAGS
  Curses LIBS...........: $CURSES_LIBS
  C Compiler............: $CC $CFLAGS $CPPFLAGS $LDFLAGS $LIBS

------------- Compiler version --------------
$($CC --version || true)
---------------------------------------------

Check the above options and compile with:
 ${MAKE-make}

EOF
