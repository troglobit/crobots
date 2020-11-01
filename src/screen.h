/* screen.h - low level screen display routines
 *
 * Copyright (C) 1985-2013 Tom Poindexter
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
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

/**
 * Local Variables:
 *  indent-tabs-mode: nil
 *  c-file-style: "gnu"
 * End:
 */
