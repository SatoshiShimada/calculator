
#ifndef __CALC_H__
#define __CALC_H__

int calc(void);
int input_formula(char *, int);
int to_RPN(char *);
double calc_RPN(void);
int get_next_token(char *, int *, double *);
int init_token_type(void);
int show_usage(void);

#endif /* __CALC_H__ */

