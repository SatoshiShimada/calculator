
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

Matrix input_matrix(void);
Matrix make_matrix(int, int, double *);
Matrix inverse_matrix(Matrix);
int    show_matrix(Matrix);

int matrix_mode(void)
{
	char buf[1024];
	Matrix m;

	m = input_matrix();

	printf("choose operation\n");
	printf("inv: inverse matrix\n");
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf) - 1] = '\0'; /* delete new line */
	if(!strcmp(buf, "inv")) {
		m = inverse_matrix(m);
	}
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
	char *c, *split;
	int i, j;
	int count;
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
	for(i = 0; i < m.row; i++) {
		printf("Enter [%d/%d] row. (split space)\n", i, m.row);
		fgets(buf, sizeof(buf), stdin);
		split = buf;
		for(count = 0; count < m.column; count++) {
			m.value[j++] = atof(split);
			c = strchr(split, ' ');
			split = c + 1;
		}
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

