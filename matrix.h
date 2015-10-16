
#ifndef __MATRIX_H__
#define __MATRIX_H__

#define MAX_ENTRY_COUNT 100

typedef struct matrix_t {
	int row;
	int column;
	double value[MAX_ENTRY_COUNT];
} Matrix;

int matrix_mode(void);

#endif // __MATRIX_H__
