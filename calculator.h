
#ifndef __CALCULATOR_H__
#define __CALCULATOR_H__

/* calculator */

int calc(void);
int calc_with_formula(int, char **);
int set_angle_unit(int);
int show_usage(void);
int list_of_functions(void);

enum angle_type {
	DEGREE,
	RADIAN,
};

/* formula */

#define append_operator(y) append_formula(0,y)
#define append_value(y)    append_formula(1,y)

int append_formula(int, double);
int get_formula(double *);
int peek_formula(double *);

/* stack */

int init_stack(void);
int term_stack(void);
int push(int);
int pop(int *);

#endif /* __CALCULATOR_H__ */

