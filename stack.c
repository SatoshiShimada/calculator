
#include <stdlib.h>

typedef struct t_stack_data {
	int value;
	struct t_stack_data *prev;
} STACK_DATA;

STACK_DATA *start = NULL, *end = NULL;

/* stack memory initilize */
int init_stack(void)
{
	start = NULL;
	end = NULL;
	return 0;
}

/* stack terminate */
int term_stack(void)
{
	STACK_DATA *p;
	if(start != NULL) {
		while(end->prev != NULL) {
			p = end->prev;
			free(end);
			end = p;
		}
		free(end);
	}
	return 0;
}

int push(int value)
{
	STACK_DATA *p;
	p = (STACK_DATA *)malloc(sizeof(STACK_DATA));
	if(start == NULL) {
		start = p;
		p->prev = NULL;
	} else {
		p->prev = end;
	}
	end = p;
	p->value = value;

	return 0;
}

int pop(int *value)
{
	STACK_DATA *p;
	if(start == NULL) {
		return -1;
	} else {
		*value = end->value;
		p = end->prev;
		free(end);
		end = p;
	}
	if(end == NULL) {
		start = NULL;
	}

	return 0;
}

