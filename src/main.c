

/*****************************************************************************/
/*                                                                           */
/*  CROBOTS                                                                  */
/*                                                                           */
/*  (C) Copyright Tom Poindexter, 1985, all rights reserved.                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/* main.c - top level controller */


/* INIT causes externals in crobots.h to have storage, & init intrinsic table */
#define INIT 1 

/* C includes */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
/* crobots includes */
#include"crobots.h"
#include"compiler.h"
#include"display.h"
#include"tokens.h"
#include"cpu.h"
#include"motion.h"
#include"screen.h"




/* files declared in compiler.h */
FILE *f_in;
FILE *f_out;
/* flex input and output files */
extern FILE *yyin;
extern FILE *yyout;

/* SIGINT handler */
void catch_int(int);

/* high level functions */
void init_robot(int i);
void comp(char *f[], int n);
void trace(char *f);
void match(int m, long l, char *f[], int n);
void play(char *f[], int n);
void free_robot(int i);
void rand_pos(int n);


int main(int argc,char *argv[])
{
  long limit = CYCLE_LIMIT;
  int matches = 0;
  int comp_only = 0;
  int debug_only = 0;
  int i;
  int num_robots = 0;
  char *files[MAXROBOTS];
  char *prog;
  char *strrchr();   /* this is rindex in some compilers */
  unsigned seed;
  long time();
  long atol();
  long cur_time;
  //int srand();


  /* print version, copyright notice, GPL notice */

  fprintf(stderr,"\n");
  fprintf(stderr,"CROBOTS - version 1.1, December, 1985\n");
  fprintf(stderr,"Copyright 1985 by Tom Poindexter, All rights reserved.\n");
  fprintf(stderr,"\n     CROBOTS - fighting robots C compiler and virtual computer\n");
  fprintf(stderr,"       distributed under the GNU GPL, version 2.\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"Press <enter> to continue......");
  getchar();
  fprintf(stderr,"\n");

  /* init robots */
  for (i = 0; i < MAXROBOTS; i++) {
    init_robot(i);
    robots[i].name[0] = '\0';
  }


  /* seed the random number generator */
  cur_time = time((long *) 0);
  seed = (unsigned) (cur_time & 0x0000ffffL);
  srand(seed);

  prog = "crobots";

  /* parse the command line */
  for (i = 1; --argc; i++) {

    if (argv[i][0] == '-') {

      switch (argv[i][1]) {
       
	/* limit number of cycles in a match */
	case 'l':
	case 'L':
	  limit = atol((argv[i])+2);
	  break;

	/* run multiple matches */
        case 'm':
        case 'M':
	  matches = atoi((argv[i])+2);
	  break;
	   
	/* compile only flag */
        case 'c':
        case 'C':
          comp_only = 1;
	  break;

	/* debug one robot */
        case 'd':
        case 'D':
          debug_only = 1;
	  break;

	default:
	  break;
      }

    } else { 	/* a file name, check for existence */

      if (num_robots < MAXROBOTS) {
	if ((f_in = fopen(argv[i],"r")) != (FILE *) NULL) {
//	  fclose(f_in);
	  files[num_robots] = argv[i];
	  num_robots++;
	} else {
	  fprintf(stderr,"%s: robot source file `%s' not found\n",prog,
		  argv[i]);
          printf("Press <enter> to continue......");
          getchar();
          printf("\n");
	}
      } else {
	fprintf(stderr,"%s: extra robot source `%s' ignored\n",prog,argv[i]);
      }
    }
	
  }

  /* make sure there is at least one robot at this point */
  if (num_robots == 0) {
    fprintf(stderr,"%s: no robot source files\n",prog);
    exit(1);
  }

  /* now, figure out what to do */

  /* compile only */
  if (comp_only) {
    comp(files,num_robots);
  }
  else 

    /* debug the first robot listed */
    if (debug_only) {
      trace(files[0]); /* trace only first source */
    }
    else

      /* run a series of matches */
      if (matches != 0) {
	if (num_robots < 2) {	/* if only one robot, make it fight itself */
	  fprintf(stderr,"%s: only one robot?, cloning a second from %s.\n",
		  prog,files[0]);
	  num_robots++;
	  files[1] = files[0];
	}
	match(matches,limit,files,num_robots);
      }

      /* play with full display */
      else {
	if (num_robots < 2) {
	  fprintf(stderr,"%s: only one robot?, cloning a second from %s.\n",
		  prog,files[0]);
	  num_robots++;
	  files[1] = files[0];
	}
	play(files,num_robots);
      }
	
  /* all done */ 
  exit(0);

}


/* comp - only compile the files with full info */
void comp(char *f[], int n)
{
  int i;

  f_out = stdout;
  r_debug = 1;  /* turns on full compile info */

  for (i = 0; i < n; i++) {
    fprintf(f_out,"Compiling robot source: %s\n\n",f[i]);
    f_in = fopen(f[i],"r");

    /* compile the robot */
    r_flag = 0;
    cur_robot = &robots[i];
    init_comp();	/* initialize the compiler */
    yyin=f_in;
    yyparse();		/* start compiling */
    reset_comp();	/* reset compiler and complete robot */
    fclose(f_in);

    /* check r_flag for compile errors */
    if (r_flag) {
      fprintf(stdout,"\n%s could not compile\n\n",f[i]);
    } else {
      fprintf(stdout,"\n%s compiled without errors\n\n",f[i]);
    }
    if (i < n-1) {
      fprintf(stdout,"\n\n\nPress <enter> to continue.\n");
      getchar();
    }
  }
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
  char *s;
  char *strrchr();  /* this is rindex in some implementations */
  
  //f_out = fopen("/dev/null","w");
  f_out=stdout;
  r_debug = 0;  /* turns off full compile info */

  for (i = 0; i < n; i++) {
    fprintf(f_out,"Compiling robot source: %s\n\n",f[i]);
    f_in = fopen(f[i],"r");

    /* compile the robot */
    r_flag = 0;
    cur_robot = &robots[num_robots];

    init_comp();	/* initialize the compiler */
    yyin = f_in;
    yyparse();		/* start compiling */
    reset_comp();	/* reset compiler and complete robot */
    fclose(f_in);

    /* check r_flag for compile errors */
    if (r_flag) {
      fprintf(stdout,"\n %s could not compile\n",f[i]);
      free_robot(num_robots);
    } else {
      fprintf(stdout,"\n %s compiled without errors\n",f[i]);
      /* get last part of file name */
      s = strrchr(f[i],'/');

      if (s == (char *) NULL)
        s = f[i];
      strcpy(robots[num_robots].name,s);
      robot_go(&robots[num_robots]);
      num_robots++;
    }
    fprintf(stdout,"\n\nPress <enter> to continue.\n");
    getchar();
  }

  if (num_robots < 2) {
    fprintf(stdout,"\n\nCannot play without at least 2 good robots.\n\n");
    exit(1);
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
	if (missiles[i][j].stat == FLYING) {
	  k = 1;
	}
      }
    }
    if (k) {
      move_robots(1);
      move_miss(1);
      update_disp();
    } 
    else  
      break;
  }

  end_disp();

  k = 0;
  for (i = 0; i < MAXROBOTS; i++) {
    if (robots[i].status == ACTIVE) {
      fprintf(stdout,"\r\nAnd the winner is: ");
      fprintf(stdout,"(%d) %s\r\n",i+1,robots[i].name);
      k = 1;
      break;
    }
  }

  if (k == 0) {
    fprintf(stdout,"\r\nIt's a draw\r\n");
  }

  //end_disp();
  exit(0);

}




/* match - run a series of matches */
void match(int m, long l, char *f[], int n)
{
  int num_robots = 0;
  int robotsleft;
  int m_count;
  int movement;
  int i, j, k;
  int wins[MAXROBOTS];
  int ties[MAXROBOTS];
  long c;
  char *s;
  char *strrchr();  /* this is rindex in some implementations */

  f_out = fopen("/dev/null","w");
  r_debug = 0;  /* turns off full compile info */

  for (i = 0; i < n; i++) {
    wins[i] = 0;
    ties[i] = 0;
    fprintf(stderr,"Compiling robot source: %s\n",f[i]);
    f_in = fopen(f[i],"r");

    /* compile the robot */
    r_flag = 0;
    cur_robot = &robots[num_robots];

    /* compile the robot */
    init_comp();
    yyin=f_in;
    yyparse();
    reset_comp();
    fclose(f_in);

    /* check r_flag for compile errors */
    if (r_flag) {
      fprintf(stderr,"\n %s could not compile\n",f[i]);
      free_robot(num_robots);
    } else {
      fprintf(stderr,"\n %s compiled without errors\n",f[i]);
      /* get last part of file name */
      s = strrchr(f[i],'/');

      if (s == (char *) NULL)
        s = f[i];
      strcpy(robots[num_robots].name,s);
      num_robots++;
    }
  }

  fclose(f_out);

  if (num_robots < 2) {
    fprintf(stderr,"\n\nCannot play without at least 2 robots.\n\n");
    exit(1);
  }

  fprintf(stderr,"\nMatch play starting.\n\n");
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
	    if (missiles[i][j].stat == EXPLODING) {
	      count_miss(i,j);
	    }
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
      printf("mutual destruction\n");
    } else {
      printf("\n");
    }

    printf("  Cumulative score:\n");
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

  fprintf(stderr,"\nMatch play finished.\n\n");
  exit(0);

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



/* trace - compile and run the robot in debug mode */
void trace(char *f)
{
  int c = 1; 

  r_debug = 1; /* turns on debugging in cpu */
  f_in = fopen(f,"r");
  f_out= stdout;

  /* compile the robot */
  r_flag = 0;
  cur_robot = &robots[0];
  init_comp();
  yyin=f_in;
  yyparse();
  reset_comp();

  /* check r_flag for compile errors */
  if (r_flag) {
    fprintf(stderr," %s could not compile\n",f);
    exit(1);
  }
  else
    robot_go(&robots[0]);

  /* randomly place robot */
  robots[0].x = rand() % MAX_X * 100;
  robots[0].y = rand() % MAX_Y * 100;

  /* setup a dummy robot at the center */
  robots[1].x = MAX_X / 2 * 100;
  robots[1].y = MAX_Y / 2 * 100;
  robots[1].status = ACTIVE;

  cur_robot = &robots[0];

  printf("\n\nReady to debug, use `d' to dump robot info, `q' to quit.\n\n");

  while (c) {  
    cycle();

    /* r_flag set by hitting 'q' in cycle()'s debug mode */
    if (r_flag)
      c = 0;
    move_robots(0);
    move_miss(0);
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



/* free_robot - frees any allocated storage in a robot */
void free_robot(int i) 
{
  s_func *temp;

  if (robots[i].funcs != (char *) 0)
    free(robots[i].funcs);

  if (robots[i].code != (struct instr *) 0)
    free(robots[i].code);

  if (robots[i].external != (long *) 0)
    free(robots[i].external);

  if (robots[i].stackbase != (long *) 0)
    free(robots[i].stackbase);

  while (robots[i].code_list != (s_func *) 0) {
    temp = robots[i].code_list;
    robots[i].code_list = temp->nextfunc;
    free(temp);
  }

}


/* catch_int - catch the interrupt signal and die, cleaning screen */
void catch_int(int i)
{
  if (!r_debug)
    end_disp();

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
  fflush(stdout);

  exit(0);
}

/* end of main.c */
