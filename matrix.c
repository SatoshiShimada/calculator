
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

Matrix input_matrix(void);
Matrix make_matrix(int, int, double *);
Matrix inverse_matrix(Matrix);
int    show_matrix(Matrix);

int matrix_mode(void)
{
	Matrix m;
	m = input_matrix();
	m = inverse_matrix(m);
	printf("%d x %d\n"
		   "%lf %lf\n%lf %lf\n",
		   m.row, m.column,
		   m.value[0], m.value[1], m.value[2], m.value[3]);
	show_matrix(m);
	return 0;
}

Matrix inverse_matrix(Matrix m)
{
	double det, tmp;
	double idet;

	if(m.row != m.column) {
		fprintf(stderr, "Error: not calculate inverse matrix of this\n");
		return m;
	}
	if(m.row == 2 && m.column == 2) {
		det = (m.value[0] * m.value[3]) - (m.value[1] * m.value[2]);
		idet = 1 / det;
		tmp = m.value[3];
		m.value[3] = m.value[0];
		m.value[0] = tmp;
		m.value[1] *= -1;
		m.value[2] *= -1;

		m.value[0] *= idet;
		m.value[1] *= idet;
		m.value[2] *= idet;
		m.value[3] *= idet;
	}
	
	return m;
}

Matrix input_matrix(void)
{
	char buf[1024];
	int i, j;
	Matrix m;

	printf("Enter row of matrix\n");
	fgets(buf, sizeof(buf), stdin);
	m.row = atoi(buf);
	printf("Enter column of matrix\n");
	fgets(buf, sizeof(buf), stdin);
	m.column = atoi(buf);
	if(m.row * m.column > 100) {
		fprintf(stderr, "Error: over size\n");
		return m;
	}
	j = 0;
	for(i = 0; i < m.row * m.column; i++) {
		printf("Enter %d row\n", i);
		fgets(buf, sizeof(buf), stdin);
		m.value[j++] = atof(buf);
	}

	return m;
}

Matrix make_matrix(int row, int column, double array[])
{
	int r, c;
	Matrix m;

	if(row * column > MAX_ENTRY_COUNT) {
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

int show_matrix(Matrix m)
{
	int r, c;

	for(c = 0; c < m.row; c++) {
		for(r = 0; r < m.column; r++) {
			printf("%3lf ", m.value[c * m.column + r]);
		}
		printf("\n");
	}

	return 0;
}

