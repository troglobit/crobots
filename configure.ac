AC_INIT([CROBOTS], [1.1],, [crobots], [https://github.com/troglobit/crobots/])
AC_CONFIG_AUX_DIR(aux)
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

# Check for curses/ncurses compatible library
AC_CHECK_HEADERS([curses.h])
AC_SEARCH_LIBS(initscr, curses)

# Check for a common math function in -lm
AC_SEARCH_LIBS([cos], [m])

AC_OUTPUT
