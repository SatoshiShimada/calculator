
#ifndef __CALC_H__
#define __CALC_H__

int calc(void);
int calc_with_formula(int, char **);
int set_angle_unit(int);
int show_usage(void);
int list_of_functions(void);

enum angle_type {
	DEGREE,
	RADIAN,
};

#endif /* __CALC_H__ */

