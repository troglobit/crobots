/*****************************************************************************/
/*                                                                           */
/*  CROBOTS                                                                  */
/*                                                                           */
/*  (C) Copyright Tom Poindexter, 1985, all rights reserved.                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef CROBOTS_COMPILER_H_
#define CROBOTS_COMPILER_H_

#include <stdio.h>
#include "crobots.h"

/* compiler variables */

#define MAXSYM    64    /* maximum number of symbol table entries per pool */
#define NESTLEVEL 16	/* maximum nest level for ifs, whiles, and fcalls */

extern char *yytext;	/* from lexical analyzer */
extern int   yylineno;	/* from lexical analyzer */
extern FILE *yyin,	/* flex input and output files */
	    *yyout;

extern FILE *f_in,	/* the compiler input source file */
            *f_out;	/* the compiler diagnostic file, assumed opened */

extern
char last_ident[],	/* last identifier recognized */
     func_ident[];	/* used on function definitions */
/*TODO: include mainheim */
     
extern
s_instr *last_ins,	/* last instruction compiled */
        *instruct;	/* current instruction */

extern
long kk;		/* constant */

extern
int num_parm,		/* number of parameters in a function definition */
    un_op,		/* for special unary operators */
    num_instr,		/* counts number of instructions */
    column,		/* from lexical analyzer */
    if_nest,		/* current if nest level */
    undeclared,		/* count variables that are implicit */
    postfix;		/* count the usage of postfix operators */

extern
char *ext_tab,		/* external symbol table */
     *var_tab,		/* local symbol table */
     *func_tab,		/* function table */
     *func_stack,	/* function call stack */
     *var_stack;	/* variable stack */

extern
int  func_off,		/* function stack offset */
     var_off,		/* variable stack offset */
     *op_stack,		/* assignment operator stack */
     op_off,		/* assignment operator offset */
     work,		/* integer work value */
     while_nest,	/* current while nest level */
     in_func;		/* in or not in function body, for variable declares */

extern
s_func *nf;		/* current function header */

struct intrin {
  char *n;
  void (*f)();
};

extern struct intrin intrinsics[];


/* functions */
void yyerror(char *s);
int yyparse(void);
int yylex_destroy(void);

void init_comp(void);
int reset_comp(void);

int new_func(void);
void end_func(void);

int allocvar(char s[], char *pool);
int findvar(char s[], char *pool);

int stackid(char id[], char *stack, int *ptr);
int popid(char id[], char *stack, int *ptr);

int poolsize(char *pool);
void dumpoff(char *pool);

int efetch(int offset);
int estore(int offset, int op);

int econst(long c);
int ebinop(int c);
int efcall(int c);

int eretsub(void);
int ebranch(void);

int echop(void);
int eframe(void);

int new_if(void);
int else_part(void);
void close_if(void);

int new_while(void);
int while_expr(void);
int close_while(void);

void decompile(s_instr * code);
void decinstr(s_instr *code);

void printop(int op);

#endif /* CROBOTS_COMPILER_H_ */
