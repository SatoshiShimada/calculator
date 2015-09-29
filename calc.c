
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "stack.h"
#include "formula.h"

int main(int argc, char *argv[])
{
	while(!calc()) ;

	return 0;
}

int input_formula(char *formula_str, int str_size)
{
	char buf0[1024];
	char buf1[1024];
	char *str;
	int i;

	fgets(buf0, sizeof(buf0), stdin);
	for(str = buf0; *str != '\0'; str++) {
		if(*str == '\n')
			*str = '\0';
	}

	for(str = buf0, i = 0; *str != '\0'; str++) {
		if(*str != ' ') {
			buf1[i++] = *str;
		}
	}
	buf1[i] = '\0';
	strncpy(formula_str, buf1, str_size);

	return 0;
}

int to_RPN(char *src) {
	char buf0[1024];
	char *str, *str_start;
	int i;
	char c;

	init_stack();

	str_start = buf0;
	strncpy(buf0, src, sizeof(buf0));
	for(str = buf0; *str != '\0'; str++) {
		c = *str;
		if(c == '+' || c == '-' || c == '*' || c == '/') {
			*str = '\0';
			append_value(atoi(str_start));
			str_start = str + 1;

			if(pop(&i)) {
				push(c);
				continue;
			}
			push(i);
			if(i == '*' || i == '/') {
				if(c == '+' || c == '-') {
					while(!pop(&i)) {
						append_operator(i);
					}
				}
				if(c == '*' || c == '/') {
					while(!pop(&i)) {
						append_operator(i);
					}
				}
			}
			if(i == '+' || i == '-') {
				if(c == '+' || c == '-') {
					while(!pop(&i)) {
						append_operator(i);
					}
				}
				if(c == '*' || c == '/') {
				}
			}
			push((int)c);
		}
	}
	append_value(atoi(str_start));

	while(!pop(&i)) {
		append_operator(i);
	}

	term_stack();
	return 0;
}

int calc_RPN(void)
{
	int ret, data;
	int i, j;

	init_stack();
	for(;;) {
		ret = get_formula(&data);
		if(ret == 1) {
			push(data);
		} else if(ret == 0) {
			switch(data) {
			case '+':
				pop(&j);
				pop(&i);
				push(i + j);
				break;
			case '-':
				pop(&j);
				pop(&i);
				push(i - j);
				break;
			case '*':
				pop(&j);
				pop(&i);
				push(i * j);
				break;
			case '/':
				pop(&j);
				pop(&i);
				push(i / j);
				break;
			}
		} else if(ret == -1) {
			break;
		}
	}
	pop(&ret);
	term_stack();
	return ret;
}

int calc(void)
{
	char buf[1024];
	int ret;

	printf("> ");
	input_formula(buf, sizeof(buf));
	to_RPN(buf);
	ret = calc_RPN();
	printf("%d\n", ret);

	return 0;
}

