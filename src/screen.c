/*****************************************************************************/
/*                                                                           */
/*  CROBOTS                                                                  */
/*                                                                           */
/*  (C) Copyright Tom Poindexter, 1985, all rights reserved.                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/* screen.c - low level screen display routines */
/*            change or modify this module for different systems */

#include "crobots.h"
#include "screen.h"

/* playfield characters */
#define UL_CORN ACS_ULCORNER
#define UR_CORN ACS_URCORNER
#define LL_CORN ACS_LLCORNER
#define LR_CORN ACS_LRCORNER
#define VERT    ACS_VLINE
#define HORZ    ACS_HLINE
#define T_TEE   ACS_TTEE
#define B_TEE   ACS_BTEE

/* exploding shell characters */
#define CENTER  ' '
#define DIAG_R  '/'
#define DIAG_L  '\\'
#define SIDE    '-'
#define TOP_BOT '|'
#define SHELL   ACS_DIAMOND



/* structure for explosions */
struct {
  int xx;
  int yy;
  int val;
} exp_pos [9] = {
  {  0,  0, CENTER  },
  { -1,  0, SIDE    },
  {  1,  0, SIDE    },
  {  0, -1, TOP_BOT },
  {  0,  1, TOP_BOT },
  { -1, -1, DIAG_L  },
  {  1, -1, DIAG_R  },
  { -1,  1, DIAG_R  },
  {  1,  1, DIAG_L  }
};

#define STAT_WID 20   /* width of characters for status boxes */

static int f_width;  /* play field width */
static int f_height; /* play field height */


/* init_disp - initialize display */
void init_disp(void)
{
  initscr();
  /* color */
  start_color();
  init_pair(1 , (short)A_BOLD | COLOR_BLACK,  COLOR_RED    );
  init_pair(2 , COLOR_BLACK,  COLOR_GREEN  );
  init_pair(3 , COLOR_BLACK,  COLOR_YELLOW );
  init_pair(4 , COLOR_BLACK,  COLOR_BLUE   );
  init_pair(5 , COLOR_RED,    COLOR_BLACK  );
  init_pair(6 , COLOR_GREEN,  COLOR_BLACK  );
  init_pair(7 , COLOR_YELLOW, COLOR_BLACK  );
  init_pair(8 , COLOR_BLUE,   COLOR_BLACK  );

  curs_set(0);
  clear();
  crmode();
  noecho();
  nonl();
  draw_field();
}


/* end_disp - cleanup and end display */

void end_disp(void)
{
  nocrmode();
  echo();
  nl();
  refresh();
  endwin();
  curs_set(1);
}


/* draw_field - draws the playing field and status boxes */

void draw_field(void)
{
  int i;

  /* init fixed screen data; 0,0 is top left, LINES-1,COLS-1 is lower right */
  f_width = COLS - STAT_WID - 3;  /* columns available */
  f_height= LINES - 3;            /* lines available */

  
  /* top line */
  move(0,0);
  addch(UL_CORN);
  for (i = 0; i <= f_width; i++) {
    addch(HORZ);
  }
  addch(UR_CORN);

  /* middle lines */
  for (i = 1; i <= f_height+1; i++) {
    move(i,0);
    addch(VERT);
    move(i,COLS-STAT_WID-1);
    addch(VERT);
  }

  /* bottom line */
  move(LINES-1,0);
  addch(LL_CORN);
  for (i = 0; i <= f_width; i++) {
    addch(HORZ);
  }
  addch(LR_CORN);

  /* status boxes -- CAUTION: this is dependent on MAXROBOTS */
  for (i = 0; i < MAXROBOTS; i++) {
    move(5*i+1,COLS-STAT_WID+1);
    attron(COLOR_PAIR(i+1));addch('1'+i);attroff(COLOR_PAIR(i+1));
    printw(" %-14s",robots[i].name);
  }
  
  move(LINES-1,COLS-STAT_WID);
  printw(" CPU Cycle:       ");

  refresh();
  
}



/* plot_robot - plot the robot position */

void plot_robot(int n)
{
  int i, k;
  register int new_x, new_y;

  new_x = (int) (((long)((robots[n].x+(CLICK/2)) / CLICK) * f_width) / MAX_X);
  new_y = (int) (((long)((robots[n].y+(CLICK/2)) / CLICK) * f_height) / MAX_Y);
  /* add one to x and y for playfield offset in screen, and inverse y */
  new_x++;
  new_y = f_height - new_y;
  new_y++;

  if (robots[n].last_x != new_x || robots[n].last_y != new_y) {
    /* check for conflict */
    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (i == n || robots[n].status == DEAD)
	continue; /* same robot as n or inactive */
      if (new_x == robots[i].last_x && new_y == robots[i].last_y) {
	k = 0;
	break;    /* conflict, robot in that position */
      }
    }
    if (k) {
      if (robots[n].last_y >= 0) {
        move(robots[n].last_y,robots[n].last_x);
        addch(' ');
      }
      move(new_y,new_x);
      attron(COLOR_PAIR(n+1));
      addch(n+'1');  /* ASCII dependent */
      attroff(COLOR_PAIR(n+1));
      refresh();
      robots[n].last_x = new_x;
      robots[n].last_y = new_y;
    }
  }
}


/* plot_miss - plot the missile position */
void plot_miss(int r, int n)
{
  int i, k;
  register int new_x, new_y;

  new_x = (int) (((long)((missiles[r][n].cur_x+(CLICK/2)) / CLICK) 
		  * f_width) / MAX_X);
  new_y = (int) (((long)((missiles[r][n].cur_y+(CLICK/2)) / CLICK) 
		  * f_height) / MAX_Y);
  /* add one to x and y for playfield offset in screen, and inverse y */
  new_x++;
  new_y = f_height - new_y;
  new_y++;

  if (missiles[r][n].last_xx != new_x || missiles[r][n].last_yy != new_y) {
    /* check for conflict */
    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (robots[i].status == DEAD)
	continue; /* inactive robot */
      if ((new_x == robots[i].last_x && new_y == robots[i].last_y)  ||
          (missiles[r][n].last_xx == robots[i].last_x && 
	   missiles[r][n].last_yy == robots[i].last_y)) {
	k = 0;
	break;    /* conflict, robot in that position */
      }
    }
    if (k) {
      if (missiles[r][n].last_yy > 0) {
        move(missiles[r][n].last_yy,missiles[r][n].last_xx);
        addch(' ');
      }
      move(new_y,new_x);
      attron(COLOR_PAIR(MAXROBOTS + r + 1));
      addch(SHELL);
      attroff(COLOR_PAIR(MAXROBOTS + r + 1));
      refresh();
      missiles[r][n].last_xx = new_x;
      missiles[r][n].last_yy = new_y;
    }
  }
}



/* plot_exp - plot the missile exploding */

void plot_exp(int r, int n)
{
  int c, i, p, hold_x, hold_y, k;
  register int new_x, new_y;

  if (missiles[r][n].count == EXP_COUNT) {
    p = 1;  /* plot explosion */
    /* erase last missile postion */
    /* check for conflict */
    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (robots[i].status == DEAD)
	continue; /* inactive robot */
      if (missiles[r][n].last_xx == robots[i].last_x && 
	  missiles[r][n].last_yy == robots[i].last_y) {
	k = 0;
	break;    /* conflict, robot in that position */
      }
    }
    if (k) {
      if (missiles[r][n].last_yy > 0) {
        move(missiles[r][n].last_yy,missiles[r][n].last_xx);
        addch(' ');
      }
    }
  }
  else
    if (missiles[r][n].count == 1)
      p = 0; /* last count, remove explosion */
    else
      return;  /* continue to display explosion */

  hold_x = (int) (((long)((missiles[r][n].cur_x+(CLICK/2)) / CLICK) 
		   * f_width) / MAX_X);
  hold_y = (int) (((long)((missiles[r][n].cur_y+(CLICK/2)) / CLICK) 
		   * f_height) / MAX_Y);

  for (c = 0; c < 9; c++) {
    new_x = hold_x + exp_pos[c].xx;
    new_x++;
    new_y = f_height - hold_y + exp_pos[c].yy;
    new_y++;

    /* check for off of playfield */
    if (new_x <= 0 || new_x > f_width+1 || new_y <= 0 || new_y > f_height+1)
      continue;

    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (robots[i].status == DEAD) 
	continue; 
      if (new_x == robots[i].last_x && new_y == robots[i].last_y) {
	k = 0;
	break;    /* conflict */
      }
    }
    if (k) {
      move(new_y,new_x);
      addch((p) ? exp_pos[c].val : ' ');
    }
  }
  refresh();
}


/* robot_stat - update status info */
void robot_stat(int n)
{
  int changed = 0;
  int d,i;

  if (robots[n].last_damage != robots[n].damage) {
    d=robots[n].damage*(STAT_WID-2)/100;

    move(5*n+2,COLS-STAT_WID+1);
    for(i=0; i < (STAT_WID-2 - d); i++) {
      addch(ACS_HLINE);
    }
    for(; i < STAT_WID-2; i++) {
      addch(' ');
    }
    move(5*n+3,COLS-STAT_WID+1);
    printw("%03d",robots[n].damage);

    robots[n].last_damage = robots[n].damage;
    changed = 1;
  }

  move(5*n+3,COLS-STAT_WID+5);
  printw("(%3d,",robots[n].x / CLICK);
  printw("%3d)",robots[n].y / CLICK);

  if (changed)
    refresh();
}


void show_cycle(long l)
{
  move(LINES-1,COLS-7);
  printw("%7ld",l);
  refresh();
}

