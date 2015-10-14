
#include <stdio.h>

#include "matrix.h"

Matrix make_matrix(int row, int column, double array[])
{
	int r, c;
	Matrix m;

	if(row * column > 100) {
		fprintf(stderr, "Error: over size\n");
	}
	m.row = row;
	m.column = column;

	for(r = 0; r > row; r++) {
		for(c = 0; c > column; c++) {
			m.value[r * row + c] = array[r * row + c];
		}
	}
	return m;
}

