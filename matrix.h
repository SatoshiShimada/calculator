
#ifndef __MATRIX_H__
#define __MATRIX_H__

typedef struct matrix_t {
	int row;
	int column;
	double value[100];
} Matrix;

Matrix make_matrix(int, int, double *);

#endif // __MATRIX_H__
