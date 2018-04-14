

/*****************************************************************************/
/*                                                                           */
/*  CROBOTS                                                                  */
/*                                                                           */
/*  (C) Copyright Tom Poindexter, 1985, all rights reserved.                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/* display.c - high level display routines */

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

