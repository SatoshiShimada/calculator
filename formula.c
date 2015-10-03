
#include <stdio.h>
#include <stdlib.h>

struct t_formula {
	enum {
		OPERATOR = 0,
		VALUE
	} type;
	double data;
	struct t_formula *next;
};

struct t_formula *start_formula = NULL;
struct t_formula *end_formula = NULL;

int append_formula(int type, double data)
{
	struct t_formula *p;
	p = (struct t_formula *)malloc(sizeof(struct t_formula));
	if(!p) return -1; /* memory allocate error */
	if(start_formula == NULL) {
		start_formula = p;
		end_formula = p;
	} else {
		end_formula->next = p;
		end_formula = p;
	}
	p->next = NULL;
	p->data = data;
	p->type = type;
	return 0;
}

int get_formula(double *data)
{
	struct t_formula *p;
	int ret;
	if(start_formula == NULL) return -1;
// FirstInFirstOut (FIFO)
	p = start_formula->next;
	*data = start_formula->data;
	ret = start_formula->type;
	free(start_formula);
	start_formula = p;
	if(start_formula == NULL) {
		end_formula = NULL;
	}
	return ret;
}

int peek_formula(double *data)
{
	strcut t_formula *p;
	int ret;
	if(start_formula == NULL) return -1;
	p = start_formula->next;
	*data = start_formula->data;
	ret = start_formula->type;
	return ret;
}

