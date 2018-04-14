# makefile for crobots

OBJS =  main.o grammar.o lexicon.o compiler.o cpu.o library.o display.o screen.o motion.o

SOURCES = crobots.h compiler.h tokens.h main.c grammar.y grammar.c lexicon.l lexicon.c compiler.c cpu.c library.c display.c screen.c screend.c motion.c


CFLAGS = -O -DUNIX -g -ggdb
#LIBS = -lm -lcurses -ltermlib
LIBS = -lm -lcurses

crobots: 	$(OBJS)
		gcc $(CFLAGS) $(OBJS) -o crobots $(LIBS)

grammar.o: 	crobots.h compiler.h grammar.y
		bison --defines=tokens.h -o grammar.c grammar.y
		gcc $(CFLAGS) -c grammar.c

main.o: 	crobots.h main.c grammar.o
		gcc $(CFLAGS) -c main.c

lexicon.o: 	crobots.h compiler.h lexicon.l grammar.o
		flex -B -o lexicon.c lexicon.l
		gcc $(CFLAGS) -c lexicon.c

compiler.o: 	crobots.h compiler.h compiler.c grammar.o
		gcc $(CFLAGS) -c compiler.c

cpu.o: 		crobots.h cpu.c grammar.o
		gcc $(CFLAGS) -c cpu.c

library.o:	crobots.h library.c
		gcc $(CFLAGS) -c library.c

display.o:	crobots.h display.c
		gcc $(CFLAGS) -c display.c

screen.o:	crobots.h screen.c
		gcc $(CFLAGS) -c screen.c

motion.o:	crobots.h motion.c
		gcc $(CFLAGS) -c motion.c

clean:
		rm crobots $(OBJS) grammar.c tokens.h lexicon.c
