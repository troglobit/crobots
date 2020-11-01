/* display.c - high level display routines
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
#include "crobots.h"
#include "screen.h"
#include "display.h"

/* update_disp - update all robots and missiles */

void update_disp(void)
{
  register int i, j;

  /* plot each live robot and update status */
  for (i = 0; i < MAXROBOTS; i++) {
    if (robots[i].status != DEAD) {
      plot_robot(i);
      robot_stat(i);
    }
    /* plot each missile */
    for (j = 0; j < MIS_ROBOT; j++) {
      switch (missiles[i][j].stat) {
	case AVAIL:
	  break;
	case FLYING:
	  plot_miss(i,j);
	  break;
	case EXPLODING:
	  plot_exp(i,j);
	  count_miss(i,j);
	  break;
	default:
	  break;
      }
    }
  }
}


/* count_miss - update the explosion counter */

void count_miss(int i, int j) 
{
  if (missiles[i][j].count <= 0)
    missiles[i][j].stat = AVAIL;
  else
    missiles[i][j].count--;
}

/**
 * Local Variables:
 *  indent-tabs-mode: nil
 *  c-file-style: "gnu"
 * End:
 */
