#ifndef CROBOTS_SCREEN_H_
#define CROBOTS_SCREEN_H_

#include "config.h"

#if defined HAVE_NCURSESW_CURSES_H
#  include <ncursesw/curses.h>
#elif defined HAVE_NCURSESW_H
#  include <ncursesw.h>
#elif defined HAVE_NCURSES_CURSES_H
#  include <ncurses/curses.h>
#elif defined HAVE_NCURSES_H
#  include <ncurses.h>
#elif defined HAVE_CURSES_H
#  include <curses.h>
#else
#  error "SysV or X/Open-compatible Curses header file required"
#endif

void init_disp  (void);
void end_disp   (void);

void draw_field (void);

void plot_robot (int n);
void plot_miss  (int r, int n);
void plot_exp   (int r, int n);

void robot_stat (int n);
void show_cycle (long l);

#endif /* CROBOTS_SCREEN_H_ */
