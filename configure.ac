AC_INIT([CROBOTS], [1.2],, [crobots], [https://github.com/troglobit/crobots/])
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

# Check for pkg-config first, warn if it's not installed
PKG_PROG_PKG_CONFIG

# Check for a common math function in -lm
AC_SEARCH_LIBS([cos], [m])

AX_WITH_CURSES
AS_IF([test "x$ax_cv_curses" != "xyes" ], [AC_MSG_ERROR([curses library not found])])

AC_ARG_WITH(instructions,
        AS_HELP_STRING([--with-instructions=MAX], [Cheat, CPU instructions for robots, default: 1000]),
	[codespace=$withval], [codespace=1000])

AC_ARG_WITH(stack,
        AS_HELP_STRING([--with-stack=MAX], [Cheat, data stack entries for robots, default: 500]),
	[dataspace=$withval], [dataspace=500])

AC_ARG_WITH(symbols,
        AS_HELP_STRING([--with-symbols=MAX], [Cheat, symbol entries per pool for robots, default: 64]),
	[symspace=$withval], [symspace=64])

AS_IF([test "x$codespace" != "xno"], [
	AS_IF([test "x$codespace" = "xyes"], [
		AC_DEFINE_UNQUOTED(INSTRMAX, 1000, [Max CPU instructions for robots])])
	AC_DEFINE_UNQUOTED(INSTRMAX, $codespace, [Max CPU instructions for robots])])

AS_IF([test "x$dataspace" != "xno"], [
	AS_IF([test "x$dataspace" = "xyes"], [
		AC_DEFINE_UNQUOTED(DATAMAX, 500, [Max data stack entries for robots])])
	AC_DEFINE_UNQUOTED(DATAMAX, $dataspace, [Max data stack for robots])])

AS_IF([test "x$symspace" != "xno"], [
	AS_IF([test "x$symspace" = "xyes"], [
		AC_DEFINE_UNQUOTED(SYMAX, 64, [Max number of symbol table entries per pool])])
	AC_DEFINE_UNQUOTED(SYMAX, $symspace, [Max number of symbol table entries per pool])])

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

Behavior:
  Max CPU instructions..: $codespace
  Max data stack entries: $dataspace
  Max symbols per pool..: $symspace

------------- Compiler version --------------
$($CC --version || true)
---------------------------------------------

Check the above options and compile with:
 ${MAKE-make}

EOF
