
#ifndef __FORMULA_H__
#define __FORMULA_H__

#define append_operator(y) append_formula(0,y)
#define append_value(y)    append_formula(1,y)

int append_formula(int, double);
int get_formula(double *);

#endif // __FORMULA_H__
