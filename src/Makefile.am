bin_PROGRAMS    = crobots

BUILT_SOURCES   = grammar.h
AM_LFLAGS       = -B
AM_YFLAGS       = -d

crobots_SOURCES = main.c crobots.h compiler.c compiler.h cpu.c cpu.h display.c display.h \
		  grammar.y lexer.l library.c library.h motion.c motion.h screen.c screen.h
crobots_CFLAGS  = @CURSES_CFLAGS@
crobots_LDADD   = @CURSES_LIBS@
