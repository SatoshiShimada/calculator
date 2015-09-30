
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "stack.h"
#include "formula.h"

typedef enum {
	UNSET = 0,
	END_OF_FILE = 10,
	SPACE,
	NUMBER,
	OPERATOR,
	CHAR,
	STRING,
	SYMBOL,
	KEYWORD,
	SPLIT,
} Token_type;

char token_type[sizeof(char)];

int main(int argc, char *argv[])
{
	while(!calc()) ;

	return 0;
}

int input_formula(char *formula_str, int str_size)
{
	char *buf0, *buf1;
	char *str;
	int i;
	const int buf_size = str_size + 1024;

	buf0 = (char *)malloc(buf_size);
	if(!buf0) return -1;
	buf1 = (char *)malloc(buf_size);
	if(!buf1) return -1;

	fgets(buf0, buf_size, stdin);
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

	free(buf0);
	free(buf1);

	return 0;
}

int get_next_token(char *src, int *val)
{
	int value;
	int ret;
	static int index;
	char c;
	char *p = src;

	c = src[index];
	index++;
	if(c == '\0') {
		index = 0;
		return 0;
	}
	switch(token_type[c]) {
	case NUMBER:
		value = 0;
		value = c - '0';
		for(src += index; *src != '\0'; src++, index++) {
			if(token_type[*src] != NUMBER) {
				break;
			}
			value = value * 10 + (*src - '0');
		}
		ret = 1; // NUMBER
		break;
	case OPERATOR:
		value = c;
		ret = 2; // OPERATOR
		break;
	case SPLIT:
		value = c;
		ret = 3; // SPLIT
		break;
	default:
		fprintf(stderr, "Unknown charactor [%c]\n", c);
		ret = -1;
		break;
	}
	*val = value;
	return ret;
}

int init_token_type(void)
{
	int i;
	static int f_init_type = 0;

	if(f_init_type == 1) return 0;

	for(i = 0; i < sizeof(token_type); i++) {
		token_type[i] = UNSET;
	}

	for(i = '0'; i <= '9'; i++) {
		token_type[i] = NUMBER;
	}

	for(i = 'a'; i <= 'z'; i++) {
		token_type[i] = SYMBOL;
	}

	for(i = 'A'; i <= 'Z'; i++) {
		token_type[i] = SYMBOL;
	}

	token_type['_'] = SYMBOL;

	token_type[' '] = SPACE;
	token_type['\n'] = SPACE;
	token_type['\r'] = SPACE;
	token_type['\t'] = SPACE;

	/*
	token_type['!'] = OPERATOR;
	token_type['\"'] = OPERATOR;
	token_type['#'] = OPERATOR;
	token_type['%'] = OPERATOR;
	token_type['&'] = OPERATOR;
	token_type['\''] = OPERATOR;
	*/
	token_type['('] = SPLIT;
	token_type[')'] = SPLIT;
	token_type['*'] = OPERATOR;
	token_type['+'] = OPERATOR;
	/*
	token_type[','] = SPLIT;
	*/
	token_type['-'] = OPERATOR;
	/*
	token_type['.'] = OPERATOR;
	*/
	token_type['/'] = OPERATOR;
	/*
	token_type['@'] = OPERATOR;
	token_type['['] = SPLIT;
	token_type[']'] = SPLIT;
	token_type['{'] = SPLIT;
	token_type['}'] = SPLIT;
	token_type['\\'] = OPERATOR;
	token_type['^'] = OPERATOR;
	token_type['`'] = OPERATOR;
	token_type['|'] = OPERATOR;
	token_type['~'] = OPERATOR;
	token_type['<'] = OPERATOR;
	token_type['>'] = OPERATOR;
	token_type[':'] = SPLIT;
	token_type[';'] = SPLIT;
	token_type['='] = OPERATOR;
	token_type['?'] = OPERATOR;
	*/
	token_type[0] = END_OF_FILE;

	f_init_type = 1;

	return 0;
}

int order(int ch)
{
	switch(ch) {
	case '*':
	case '/':
		return 3;
	case '+':
	case '-':
		return 2;
	case '(':
		return 1;
	}
	return -1;
}

int to_RPN(char *src) {
	char buf0[1024];
	char *str, *str_start;
	int i;
	int top;
	int type;
	int value;
	char c;

	init_stack();

	str_start = buf0;
	strncpy(buf0, src, sizeof(buf0));
	for( ;; ) {
		type = get_next_token(buf0, &value);
		switch(type) {
		case 0: // End of String
			goto end_of_formula;
		case 1: // NUMBER
			append_value(value);
			break;
		case 2: // OPERATOR
		case 3: // SPLIT
			if(pop(&top)) {
				/* empty stack */
				if(value == ')') {
					fprintf(stderr, "Invalid syntax\n");
				}
				push(value);
			} else if(value == '(') {
				push(top);
				push(value);
			} else if(value == ')') {
				for( ;; ) {
					append_operator(top);
					if(pop(&top)) break;
					if(top == '(') break;
				}
			} else {
				if(order(value) == -1) {
					fprintf(stderr, "Syntax error\n");
					break;
				}
				while(order(top) >= order(value)) {
					append_operator(top);
					if(pop(&top)) goto end_loop;
				}
				push(top);
			end_loop:
				push(value);
			}
			break;
		default:
			break;
		}
	}
end_of_formula:

	while(!pop(&i)) {
		if(i == '(') {
			fprintf(stderr, "Invalid syntax\n");
			return -1;
		}
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

	init_token_type();
	printf("> ");
	input_formula(buf, sizeof(buf));
	to_RPN(buf);
	ret = calc_RPN();
	printf("%d\n", ret);

	return 0;
}

