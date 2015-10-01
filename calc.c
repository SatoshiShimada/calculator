
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "calc.h"
#include "stack.h"
#include "formula.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

enum func_num {
	VALUE = 0,
	SIN = 1,
	COS,
	TAN,
	SQRT,
	EXP,
	LOG,
	LOG10,
	ABS,
	PLUS = '+',
	MINUS = '-',
	MUL = '*',
	DIV = '*',
	UNKNOWN = 100,
} FUNC_NUM;

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

char token_type[0xff];
int  f_use_degree = 1; // default use degree (not use radian)

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

int symbol_to_value(char *str)
{
	int ret;

	if(!strcmp(str, "quit")) {
		exit(0);
	} else if(!strcmp(str, "sin")) {
		ret = SIN;
	} else if(!strcmp(str, "cos")) {
		ret = COS;
	} else if(!strcmp(str, "tan")) {
		ret = TAN;
	} else if(!strcmp(str, "sqrt")) {
		ret = SQRT;
	} else if(!strcmp(str, "exp")) {
		ret = EXP;
	} else if(!strcmp(str, "log")) {
		ret = LOG;
	} else if(!strcmp(str, "abs")) {
		ret = ABS;
	} else {
		ret = UNKNOWN;
	}

	return ret;
}

int get_next_token(char *src, int *val, double *d_val)
{
	static int index;
	char c;
	char buf[1024];
	int value, ret, i;
	double d_value;

	buf[0] = '\0';
	i = 0;
	c = src[index];
	index++;
	if(c == '\0') {
		index = 0;
		return 0;
	}
	switch(token_type[(int)c]) {
	case NUMBER:
		buf[i++] = c;
		for(src += index; *src != '\0'; src++, index++) {
			if(token_type[(int)*src] != NUMBER) {
				break;
			}
			buf[i++] = *src;
		}
		buf[i] = '\0';
		d_value = atof(buf);
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
	case SYMBOL:
		buf[i++] = c;
		for(src += index; *src != '\0'; src++, index++) {
			if(token_type[(int)*src] != SYMBOL) {
				break;
			}
			buf[i++] = *src;
		}
		buf[i] = '\0';
		value = symbol_to_value(buf);
		ret = 4;
		break;
	default:
		fprintf(stderr, "Unknown charactor [%c]\n", c);
		ret = -1;
		break;
	}
	*val = value;
	*d_val = d_value;
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

	token_type['('] = SPLIT;
	token_type[')'] = SPLIT;
	token_type['*'] = OPERATOR;
	token_type['+'] = OPERATOR;
	token_type['-'] = OPERATOR;
	token_type['.'] = NUMBER;
	token_type['/'] = OPERATOR;
	token_type[0] = END_OF_FILE;

	f_init_type = 1;

	return 0;
}

int order(int ch)
{
	switch(ch) {
	case SIN:
	case COS:
	case TAN:
	case SQRT:
	case EXP:
	case LOG:
	case ABS:
		return 4;
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
	int top;
	int type;
	int value;
	double d_value;

	init_stack();

	strncpy(buf0, src, sizeof(buf0));
	for( ;; ) {
		type = get_next_token(buf0, &value, &d_value);
		switch(type) {
		case 0: // End of String
			goto end_of_formula;
		case 1: // NUMBER
			append_value(d_value);
			break;
		case 2: // OPERATOR
		case 3: // SPLIT
		case 4: // SYMBOL
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

	while(!pop(&top)) {
		if(top == '(') {
			fprintf(stderr, "Invalid syntax\n");
			return -1;
		}
		append_operator(top);
	}

	term_stack();
	return 0;
}

double calc_RPN(void)
{
	int ret;
	double i, j;
	double data;
	double stack2[100];
	int index = 0;

	for(;;) {
		ret = get_formula(&data);
		if(ret == 1) {
			stack2[index++] = data;
		} else if(ret == 0) {
			switch((int)data) {
			case '+':
				j = stack2[--index];
				i = stack2[--index];
				stack2[index++] = i + j;
				break;
			case '-':
				j = stack2[--index];
				i = stack2[--index];
				stack2[index++] = i - j;
				break;
			case '*':
				j = stack2[--index];
				i = stack2[--index];
				stack2[index++] = i * j;
				break;
			case '/':
				j = stack2[--index];
				i = stack2[--index];
				if(j == 0) {
					fprintf(stderr, "Division error\n");
					return -1;
				}
				stack2[index++] = i / j;
				break;
			case SIN:
				i = stack2[--index];
				if(f_use_degree) {
					i = i * M_PI / 180.0;
				}
				stack2[index++] = sin(i);
				break;
			case COS:
				i = stack2[--index];
				if(f_use_degree) {
					i = i * M_PI / 180.0;
				}
				stack2[index++] = cos(i);
				break;
			case TAN:
				i = stack2[--index];
				if(f_use_degree) {
					i = i * M_PI / 180.0;
				}
				stack2[index++] = tan(i);
				break;
			case SQRT:
				i = stack2[--index];
				stack2[index++] = sqrt(i);
				break;
			case EXP:
				i = stack2[--index];
				stack2[index++] = exp(i);
				break;
			case LOG:
				i = stack2[--index];
				stack2[index++] = log(i);
				break;
			case ABS:
				i = stack2[--index];
				stack2[index++] = abs(i);
			}
		} else if(ret == -1) {
			break;
		}
	}
	i = stack2[--index];
	return i;
}

int calc(void)
{
	char buf[1024];
	double ret;

	init_token_type();
	printf("> ");
	if(input_formula(buf, sizeof(buf)) == -1) return -1;
	if(to_RPN(buf) == -1) return -1;
	ret = calc_RPN();
	printf("%0lf\n", ret);

	return 0;
}

