

/*****************************************************************************/
/*                                                                           */
/*  CROBOTS                                                                  */
/*                                                                           */
/*  (C) Copyright Tom Poindexter, 1985, all rights reserved.                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/



/* note-the INIT flag (or lack of it) causes extern for all but one module */

#define ILEN             8	/* length of identifiers, also in lexanal.l */
#define MAXROBOTS        4	/* maximum number of robots */
#define CODESPACE     2000	/* maximum number of machine instructions 1000 originally*/
#define DATASPACE      500	/* maximum number of data stack entries */
#define UPDATE_CYCLES   30	/* number of cycles before screen update (originally 30)*/
#define MOTION_CYCLES   15 	/* number of cycles before motion update (originally 15)*/
#define CYCLE_DELAY    200	/* microseconds of sleep to slow down things when display is on*/
#define CYCLE_LIMIT 500000L	/* limit of background cycles */
#define ROBOT_SPEED      7	/* multiplicative speed factor */
#define TURN_SPEED      50	/* maximum speed for direction change */
#define ACCEL           10	/* acceleration factor per motion cycle */

#define ACTIVE    1		/* robot active & running */
#define DEAD      0		/* dead robot */



typedef struct instr {		/* robot machine instruction */
  char ins_type;		/* instruction type */
  union {
    long k;			/* constant value */
    short int var1;		/* variable offset, function offset, operator */
    struct instr *br;		/* false branch */
    struct {
      short int var2;		/* assignment variable offset */
      short int a_op;		/* assignment operator */
    } a;
  } u;
} s_instr;

typedef struct func {		/* function header */
  struct func *nextfunc;	/* next function header in chain */
  char func_name[ILEN];		/* function name */
  s_instr *first;		/* first instruction pointer */
  int var_count;		/* number of pool variables needed */
  int par_count;		/* number of parameters expected */
} s_func;

typedef struct robot {		/* robot context */
  int status;			/* status of robot, active or dead */
  char name[14];		/* name of robot */
  int x;			/* current x location * 100 */
  int y;			/* current y location * 100 */
  int org_x;			/* orgin x location * 100 */
  int org_y;			/* orgin y location * 100 */
  int range;			/* distance traveled on this heading * 100 */
  int last_x;			/* last plotted physical x */
  int last_y;			/* last plotted physical y */
  int speed;			/* current speed, 0 - 100 */
  int last_speed;		/* last speed, 0 - 100 */
  int accel;			/* linear acceleration / de-acceleration lag */
  int d_speed;			/* desired speed */
  int heading;			/* current heading, 0 - 359 */
  int last_heading;		/* last heading, 0 - 359 */
  int d_heading;		/* desired heading, 0 - 359 */
  int damage;			/* damage sustained, percent */
  int last_damage;		/* last damage */
  int scan;			/* current scan direction */
  int last_scan;		/* last scan direction */
  int reload;			/* number of cycles between reloading */
  int ext_count;		/* size of external pool needed */
  long *external;		/* external variable pool  (Lower MEM address) ?? */
  long *local;			/* current local variables on stack */
  long *stackbase;		/* base of local & expression stack */
  long *stackend;		/* end of stack (Higher MEM address) ?? */
  long *stackptr;		/* current stack pointer, grows up */
  long *retptr;			/* return frame pointers, grow down */
  char *funcs;			/* table of function names by offset */
  s_func *code_list;		/* list of function headers */
  s_instr *code;		/* machine instructions, actually instr */
  s_instr *ip; 			/* instruction pointer */
} s_robot;



/* missile constants */
#define MIS_SPEED 500		/* how far in one motion cycle (in clicks) (originally 500)*/
#define MIS_RANGE 700 		/* maximum missile range (see MAX_X, MAX_Y */
#define MIS_ROBOT 2		/* number of active missiles per robot */
#define AVAIL  0		/* missile available for use */
#define FLYING 1		/* missile in air */
#define EXPLODING 2		/* missile exploding on ground */
#define RELOAD 15		/* motion cycles before reload */
#define EXP_COUNT 5 		/* motion cycles for exploding missile */

typedef struct missile {	/* active missiles */
  int stat;			/* missile status */
  int beg_x;			/* beginning x * 100 */
  int beg_y;			/* beginning y * 100 */
  int cur_x;			/* current x * 100 */
  int cur_y;			/* current y * 100 */
  int last_xx;			/* last plotted x */
  int last_yy;			/* last plotted y */
  int head;			/* heading, 0 - 359 */
  int count;			/* cycle count for exploding missiles */
  int rang;			/* range of missile */
  int curr_dist;		/* current distance from orgin * 100 */
} s_missile;

#ifndef INIT
extern
#endif
s_missile missiles[MAXROBOTS][MIS_ROBOT];

#ifndef INIT
extern
#endif
s_robot *cur_robot,		/* current robot */
        robots[MAXROBOTS];	/* all robots */

#ifndef INIT
extern
#endif
int r_debug,			/* debug switch */
    r_flag;			/* global flag for push/pop errors */

/* instruction types */
#define NOP    0		/* end of code marker */
#define FETCH  1		/* push(varpool(offset)) */
#define STORE  2		/* push(pop op pop) --> varpool(offset) */
#define CONST  3		/* push(constant) */
#define BINOP  4		/* pop -->y, pop -->x, push(x op y) */
#define FCALL  5		/* pop --> parmn..parm1, save ip, call */
#define RETSUB 6		/* push(returnval), restore ip */
#define BRANCH 7		/* if (pop == 0) branch --> ip*/
#define CHOP   8		/* pop --> bit bucket */
#define FRAME  9		/* frame stack pointer for call */

/* external variable flag (or'ed in or and'ed out) , also in grammar.y */
#define EXTERNAL 0x8000

/* size of battlefield, see MIS_RANGE also. note - x and y coordinates */
/* of robots and missiles are measured in CLICKS * MAX_? */
#define CLICK 10		/* 10 clicks per meter */
#define MAX_X 1000		/* 1000 meters x axis */
#define MAX_Y 1000		/* 1000 meters y axis */

/* damage factors, percent */
#define DIRECT_HIT 10
#define NEAR_HIT   5
#define FAR_HIT    3
#define COLLISION  2
/* distance of damage factors */
#define DIRECT_RANGE 5
#define NEAR_RANGE   20
#define FAR_RANGE    40



/* declare library function table */
#include "library.h"

#ifndef INIT
extern
#endif
struct intrin {
  char *n;
  void (*f)();
} intrinsics[20]

#ifdef INIT
 = {
  {"*dummy*",	(void (*)()) 0},
  {"scan",	c_scan},
  {"cannon",	c_cannon},
  {"drive",	c_drive},
  {"damage",	c_damage},
  {"speed",	c_speed},
  {"loc_x",	c_loc_x},
  {"loc_y",	c_loc_y},
  {"rand",	c_rand},
  {"sin",	c_sin},
  {"cos",	c_cos},
  {"tan",	c_tan},
  {"atan",	c_atan},
  {"sqrt",	c_sqrt},
  {"",		(void (*)()) 0} 
 }
#endif
;

/* motion functions */


/* end of crobots.h header */


