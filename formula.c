
#include <stdlib.h>

struct t_formula {
	enum {
		OPERATOR = 0,
		VALUE
	} type;
	int data;
	struct t_formula *next;
};

struct t_formula *start_formula = NULL;
struct t_formula *end_formula = NULL;

int append_formula(int type, int data)
{
	struct t_formula *p;
	p = (struct t_formula *)malloc(sizeof(struct t_formula));
	if(!p) exit(0);
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

int get_formula(int *data)
{
	struct t_formula *p, *q;
	int ret;
	if(start_formula == NULL) return -1;
#if 0
// for LastInFirstOut (LIFO)
	for(p = start_formula; p->next != NULL; p = p->next) q = p;
	*data = p->data;
	ret = (int)p->type;
	free(p);
	if(start_formula == end_formula) {
		start_formula = end_formula = NULL;
	} else {
		end_formula = q;
		end_formula->next = NULL;
	}
// for LastInFirstOut (LIFO)
#else
// for FirstInFirstOut (FIFO)
	q = start_formula->next;
	*data = start_formula->data;
	ret = start_formula->type;
	free(start_formula);
	start_formula = q;
	if(start_formula == NULL) {
		end_formula = NULL;
	}
// for FirstInFirstOut (FIFO)
#endif
	return ret;
}

