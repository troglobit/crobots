/* main.c - top level controller
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
#include "config.h"

/* C includes */
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

/* crobots includes */
#include "crobots.h"
#include "compiler.h"
#include "display.h"
#include "grammar.h"
#include "cpu.h"
#include "motion.h"
#include "screen.h"

s_missile missiles[MAXROBOTS][MIS_ROBOT];

s_robot *cur_robot,		/* current robot */
        robots[MAXROBOTS];	/* all robots */

int r_debug,			/* debug switch */
    r_flag,			/* global flag for push/pop errors */
    r_interactive,		/* enable classic 'Press <enter> to continue */
    r_stats;			/* show robot stats on exit */

FILE *f_in;			/* the compiler input source file */
FILE *f_out;			/* the compiler diagnostic file, assumed opened */

/* SIGINT handler */
void catch_int(int);

/* high level functions */
int comp(char *f[], int n);
void play(char *f[], int n);
void match(int m, long l, char *f[], int n);
void debug(char *f);
void init_robot(int i);
void clone_robot(int i);
void free_robot(int i);
void robot_stats(void);
void rand_pos(int n);

static int usage(int rc)
{
  printf("Usage:\n"
	 "  crobots [options] robot1.r [robotN.r] [>file]\n"
	 "\n"
	 "Options:\n"
	 "  -c        Compile only, produce virtual machine assembler code and\n"
	 "            symbol tables\n"
	 "  -d        Compile one program, then invoke machine level single step\n"
	 "            tracing (debugger)\n"
	 "  -h        This help text\n"
	 "  -i        Interactive mode, show code output and 'Press <enter> ..'\n"
	 "  -m NUM    Run a series of matches, were NUM is the number of matches.\n"
	 "            If '-m' is not specified, the default is to run one match\n"
	 "            and display the realtime battlefield\n"
	 "  -l NUM    Limit the number of machine CPU cycles per match when '-m'\n"
	 "            is specified.  The defaul cycle limit is 500,000\n"
	 "  -s        Show robot stats on exit\n"
	 "  -v        Show program version and exit\n"
	 "\n"
	 "Arguments:\n"
	 "  robotN.r  The file name of the CROBOTS source program(s).  Up to four\n"
	 "            robots may be specified.  If only one file is specified, it\n"
	 "            will be \"cloned\" into another, so that two robots (running\n"
	 "            the same program) will compete.  Any file name may be used,\n"
	 "            but for consistency use '.r' as the extension\n"
	 "  [>file]   Use DOS 2.0+ redirection to get a compile listing (with '-c')\n"
	 "            or to record matches (with '-m option)\n"
	 "\n"
	 );

  return rc;
}


int main(int argc,char *argv[])
{
  long limit = CYCLE_LIMIT;
  int matches = 0;
  int comp_only = 0;
  int debug_only = 0;
  int ignored = 0;
  int i, c;
  int num_robots = 0;
  unsigned seed;
  long cur_time;

  setlinebuf(stdout);

  while ((c = getopt(argc, argv, "cdhil:m:sv")) != EOF) {
      switch (c) {
        case 'c':		/* compile only flag */
          comp_only = 1;
          r_debug = 1;          /* turns on full compile info */
          break;

        case 'd':		/* debug one robot */
          debug_only = 1;
          r_debug = 1;          /* turns on full compile info */
	  break;

        case 'h':
	  return usage(0);

	case 'i':
	  r_interactive = 1;
	  break;

	case 'l':		/* limit number of cycles in a match */
	  limit = atol(optarg);
	  break;

        case 'm':		/* run multiple matches */
	  matches = atoi(optarg);
	  break;

        case 's':
	  r_stats= 1;
	  break;

        case 'v':
	  puts(PACKAGE_STRING);
	  return 0;

	default:
	  break;
      }

  }

  /* print version, copyright notice, GPL notice */
  if (r_interactive) {
    printf("CROBOTS fighting robots C compiler and virtual computer, license GNU GPL, v2\n"
           "Copyright (C) 1985-2013 Tom Poindexter\n");
    fputs("Press <enter> to continue ...", stdout);
    getchar();
    fputs("\e[1A\e[K", stdout);
  }

  /* make sure there is at least one robot at this point */
  if (optind == argc)
    errx(1, "no robot source files");

  /* init robots */
  for (i = 0; i < MAXROBOTS; i++) {
    init_robot(i);
    robots[i].name[0] = '\0';
  }

  /* seed the random number generator */
  cur_time = time(NULL);
  seed = (unsigned) (cur_time & 0x0000ffffL);
  srand(seed);

  /* now, figure out what to do */
  f_out = stdout;		/* override below */

  /* compile only */
  if (comp_only) {
    comp(&argv[optind], argc - optind);
    return 0;
  }

  /* debug the first robot listed */
  if (debug_only) {
    /* trace only first source */
    debug(argv[optind]);
    return 0;
  }

  /* run a series of matches */
  if (matches != 0)
    match(matches, limit, &argv[optind], argc - optind);
  else
    play(&argv[optind], argc - optind);

  if (r_stats)
    robot_stats();

  return 0;
}


/* comp - only compile the files with full info */
int comp(char *f[], int n)
{
  int num = 0;
  char *s;
  int i;

  for (i = 0; i < n; i++) {
    if (num >= MAXROBOTS) {
      warnx("Max robots reached, skipping robot '%s' ...", f[i]);
      continue;
    }

    f_in = fopen(f[i], "r");
    if (!f_in) {
      warnx("robot '%s' not found, skipping ...", f[i]);
      continue;
    }

    s = strrchr(f[i],'/');
    if (s)
      s++;
    else
      s = f[i];

    fprintf(f_out, "Compiling %-20s", s);

    /* compile the robot */
    r_flag = 0;
    cur_robot = &robots[num];

    init_comp();	/* initialize the compiler */
    yyin = f_in;
    yyparse();		/* start compiling */
    yylex_destroy();
    reset_comp();	/* reset compiler and complete robot */
    fclose(f_in);

    /* check r_flag for compile errors */
    if (r_flag) {
      free_robot(num);
    } else {
      strcpy(robots[num].name, s);
      num++;
    }

    if (r_interactive && i < n-1) {
      fputs("Press <enter> to continue ...", stdout);
      getchar();
      fputs("\e[1A\e[K", stdout);
    }
  }

  return num;
}

/* prepare - prepare for battle */
int prepare(char *f[], int n)
{
  int num = 0;

  num = comp(f, n);
  switch (num) {
  default:
    break;

  case 1:		   /* if only one robot, make it fight itself */
    warnx("only one robot, cloning another from %s.", f[0]);
    clone_robot(0);
    num++;
    break;

  case 0:
    errx(1, "cannot play without at least 2 good robots.");
    break;
  }

  return num;
}

/* play - watch the robots compete */
void play(char *f[], int n)
{
  int num_robots = 0;
  int robotsleft;
  int display;
  int movement;
  int i, j, k;
  long c = 0L;

  num_robots = prepare(f, n);
  for (i = 0; i < num_robots; i++)
      robot_go(&robots[i]);

  puts("\nStarting ...");
  if (r_interactive) {
    fputs("Press <enter> to continue ...", stdout);
    getchar();
    fputs("\e[1A\e[K", stdout);
  }

  /* catch interrupt */
  if (signal(SIGINT,SIG_IGN) != SIG_IGN)
    signal(SIGINT,catch_int);

  rand_pos(num_robots);

  init_disp();
  update_disp();
  movement = MOTION_CYCLES;
  display = UPDATE_CYCLES;
  robotsleft = num_robots;

  /* multi-tasker; give each robot one cycle per loop */
  while (robotsleft > 1) {
    robotsleft = 0;
    for (i = 0; i < num_robots; i++) {
      if (robots[i].status == ACTIVE) {
	robotsleft++;
        cur_robot = &robots[i];
	/* TODO simulate fixed virtual Mhz */
	usleep(CYCLE_DELAY);
	cycle();
      } 
    }
    
    /* is it time to update motion? */
    if (--movement <= 0) {
      movement = MOTION_CYCLES;
      move_robots(1);
      move_miss(1);
    }

    /* is it time to update display */
    if (--display <= 0) {
      display = UPDATE_CYCLES;
      c += UPDATE_CYCLES;
      show_cycle(c);
      update_disp();
    }
  }

  /* allow any flying missiles to explode */
  while (1) {
    k = 0;
    for (i = 0; i < num_robots; i++) {
      for (j = 0; j < MIS_ROBOT; j++) {
	if (missiles[i][j].stat == FLYING)
	  k = 1;
      }
    }

    if (!k)
      break;

    move_robots(1);
    move_miss(1);
    update_disp();
  }

  end_disp();

  for (i = 0; i < MAXROBOTS; i++) {
    if (robots[i].status == ACTIVE)
      break;
  }

  if (i == MAXROBOTS)
    puts("\nIt's a draw");
  else
    printf("\nThe winner is: %s (%d)\n", robots[i].name, i + 1);
}


/* match - run a series of matches */
void match(int m, long l, char *f[], int n)
{
  int num_robots = 0;
  int robotsleft;
  int m_count;
  int movement;
  int i, j, k;
  int wins[MAXROBOTS] = { 0 };
  int ties[MAXROBOTS] = { 0 };
  long c;

  f_out = fopen("/dev/null","w");
  num_robots = prepare(f, n);
  fclose(f_out);

  puts("\nMatch play starting.");
  if (r_interactive) {
    fputs("Press <enter> to continue ...", stdout);
    getchar();
    fputs("\e[1A\e[K", stdout);
  }

  for (m_count = 1; m_count <= m; m_count++) {
    printf("\nMatch %6d: ",m_count);

    for (i = 0; i < num_robots; i++) {
      init_robot(i);
      robot_go(&robots[i]);
      robots[i].status = ACTIVE;
    }

    rand_pos(num_robots);
    movement = MOTION_CYCLES;
    robotsleft = num_robots;
    c = 0L;
    while (robotsleft > 1 && c < l) {
      robotsleft = 0;

      for (i = 0; i < num_robots; i++) {
	if (robots[i].status == ACTIVE) {
	  robotsleft++;
	  cur_robot = &robots[i];
	  cycle();
	} 
      }

      if (--movement == 0) {
	c += MOTION_CYCLES;
	movement = MOTION_CYCLES;
	move_robots(0);
	move_miss(0);

	for (i = 0; i < num_robots; i++) {
	  for (j = 0; j < MIS_ROBOT; j++) {
	    if (missiles[i][j].stat == EXPLODING)
	      count_miss(i,j);
	  }
	}
      }
    }

    /* allow any flying missiles to explode */
    while (1) {
      k = 0;
      for (i = 0; i < num_robots; i++) {
	for (j = 0; j < MIS_ROBOT; j++) {
	  if (missiles[i][j].stat == FLYING) {
	    k = 1;
	  }
	}
      }
      if (k) {
	move_robots(0);
	move_miss(0);
      } 
      else  
	break;
    }

    printf(" cycles = %ld:\n  Survivors:\n",c);

    k = 0;
    for (i = 0; i < num_robots; i++) {
      if (robots[i].status == ACTIVE) {
	printf("   (%d)%14s: damage=%% %d  ",i+1,robots[i].name,
		robots[i].damage);
	if (i == 1)
	  printf("\n");
	else
	  printf("\t");
	k++;
      }
    }

    if (k == 0) {
      puts("mutual destruction");
    } else {
      puts("");
    }

    puts("  Cumulative score:");
    for (i = 0; i < n; i++) {
      if (robots[i].status == ACTIVE) {
	if (k == 1)
	  wins[i]++;
	else
	  ties[i]++;
      }
      printf("   (%d)%14s: wins=%d ties=%d  ",i+1,robots[i].name,
	      wins[i],ties[i]);
      if (i == 1)
	printf("\n");
      else
	printf("\t");
    }
    printf("\n");
  }

  puts("\nMatch play finished.\n");
}


/* debug - compile and run the robot in debug mode */
void debug(char *f)
{
  int c = 1; 

  if (!comp(&f, 1))
    exit(1);

  robot_go(&robots[0]);

  /* randomly place robot */
  robots[0].x = rand() % MAX_X * 100;
  robots[0].y = rand() % MAX_Y * 100;

  /* setup a dummy robot at the center */
  robots[1].x = MAX_X / 2 * 100;
  robots[1].y = MAX_Y / 2 * 100;
  robots[1].status = ACTIVE;

  cur_robot = &robots[0];

  puts("\nReady to debug, use `d' to dump robot info, `q' to quit.");

  while (c) {  
    cycle();

    /* r_flag set by hitting 'q' in cycle()'s debug mode */
    if (r_flag)
      c = 0;

    move_robots(0);
    move_miss(0);
  }
}


/* rand_pos - randomize the starting robot postions */
/*           dependent on MAXROBOTS <= 4 */
/*            put robots in separate quadrant */
void rand_pos(int n)
{
  int i, k;
  int quad[4];

  for (i = 0; i < 4; i++) {
    quad[i] = 0;
  }

  /* get a new quadrant */
  for (i = 0; i < n; i++) {
    k = rand() % 4;
    if (quad[k] == 0)
      quad[k] = 1;
    else {
      while (quad[k] != 0) {
	if (++k == 4)
	  k = 0;
      }
      quad[k] = 1;
    }
    robots[i].org_x = robots[i].x =
       (rand() % (MAX_X * CLICK / 2)) + ((MAX_X * CLICK / 2) * (k%2));
    robots[i].org_y = robots[i].y =
       (rand() % (MAX_Y * CLICK / 2)) + ((MAX_Y * CLICK / 2) * (k<2));
  }
}


/* init a robot */
void init_robot(int i)
{
  register int j;

  robots[i].status = DEAD;
  robots[i].x = 0;
  robots[i].y = 0;
  robots[i].org_x = 0;
  robots[i].org_y = 0;
  robots[i].range = 0;
  robots[i].last_x = -1;
  robots[i].last_y = -1;
  robots[i].speed = 0;
  robots[i].last_speed = -1;
  robots[i].accel = 0;
  robots[i].d_speed = 0;
  robots[i].heading = 0;
  robots[i].last_heading = -1;
  robots[i].d_heading = 0;
  robots[i].damage = 0;
  robots[i].last_damage = -1;
  robots[i].scan = 0;
  robots[i].last_scan = -1;
  robots[i].reload = 0;
  for (j = 0; j < MIS_ROBOT; j++) {
    missiles[i][j].stat = AVAIL;
    missiles[i][j].last_xx = -1;
    missiles[i][j].last_yy = -1;
  }
}


/* clone_robot - create a clone when there is only one */
void clone_robot(int i)
{
  if (i + 1 >= MAXROBOTS)
    errx(1, "Robot overflow\n");

  robots[i + 1] = robots[i];
  robots[i + 1].external = (long *) malloc(robots[i].ext_count * sizeof(long));
  robots[i + 1].stackbase = (long *) malloc(DATASPACE * sizeof(long));
  robots[i + 1].stackend = robots[i + 1].stackbase + DATASPACE;
}


/* free_robot - frees any allocated storage in a robot */
void free_robot(int i) 
{
  s_func *temp;

  if (robots[i].funcs)
    free(robots[i].funcs);

  if (robots[i].code)
    free(robots[i].code);

  if (robots[i].external)
    free(robots[i].external);

  if (robots[i].stackbase)
    free(robots[i].stackbase);

  while (robots[i].code_list) {
    temp = robots[i].code_list;
    robots[i].code_list = temp->nextfunc;
    free(temp);
  }
}


/* robot_stats - dump robot stats, optionally showed at exit */
void robot_stats(void)
{
  int i;

  for (i = 0; i < MAXROBOTS; i++) {
    cur_robot = &robots[i];
    printf("\nrobot: %d",i);
    printf("\tstatus......%d",cur_robot->status);
    printf("\nx...........%5d",cur_robot->x);
    printf("\ty...........%5d",cur_robot->y);
    printf("\norg_x.......%5d",cur_robot->org_x);
    printf("\torg_y.......%5d",cur_robot->org_y);
    printf("\nrange.......%5d",cur_robot->range);
    printf("\tspeed.......%5d",cur_robot->speed);
    printf("\nd_speed.....%5d",cur_robot->d_speed);
    printf("\theading.....%5d",cur_robot->heading);
    printf("\nd_heading...%5d",cur_robot->d_heading);
    printf("\tdamage......%5d",cur_robot->damage);
    printf("\nmiss[0]stat.%5d",missiles[cur_robot-&robots[0]][0].stat);
    printf("\tmiss[1]stat.%5d",missiles[cur_robot-&robots[0]][1].stat);
    printf("\nmiss[0]head.%5d",missiles[cur_robot-&robots[0]][0].head);
    printf("\tmiss[1]head.%5d",missiles[cur_robot-&robots[0]][1].head);
    printf("\nmiss[0]x....%5d",missiles[cur_robot-&robots[0]][0].cur_x);
    printf("\tmiss[1]y....%5d",missiles[cur_robot-&robots[0]][1].cur_y);
    printf("\nmiss[0]dist.%5d",missiles[cur_robot-&robots[0]][0].curr_dist);
    printf("\tmiss[1]dist.%5d",missiles[cur_robot-&robots[0]][1].curr_dist);
    printf("\n\n");
  }
}

/* catch_int - catch the interrupt signal and die, cleaning screen */
void catch_int(int signo)
{
  (void)signo;

  if (!r_debug)
    end_disp();

  warnx("Aborted.");
  if (r_stats)
    robot_stats();

  exit(0);
}

/**
 * Local Variables:
 *  indent-tabs-mode: nil
 *  c-file-style: "gnu"
 * End:
 */
