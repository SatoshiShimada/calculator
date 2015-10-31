
/*
 * Calculator by using Reverse Polish Notation.
 *
 * features:
 *   Usable brackets, variable and functions.
 *
 * Encoding:
 *   UTF-8 (JAPANESE)
 *
 * Author:
 *   Satoshi Shimada
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "calculator.h"

#ifndef M_PI
	#define M_PI 3.141592653589793
#endif

#define JAPANESE /* enable japanese error message */

int main(int argc, char *argv[])
{
	char buf[1024];

	if(argc == 2) {
		if(!strcmp(argv[1], "-h") ||
		   !strcmp(argv[1], "--help")) {
			show_usage();
			return 0;
		} else if(!strcmp(argv[1], "-l") ||
				  !strcmp(argv[1], "--list")) {
			  list_of_functions();
		} else if(!strcmp(argv[1], "-r") ||
				  !strcmp(argv[1], "--radian")) {
			set_angle_unit(RADIAN);
		} else if(!strcmp(argv[1], "-d") ||
				  !strcmp(argv[1], "--degree")) {
			set_angle_unit(DEGREE);
		} else {
			goto unknown_option;
		}
	} else if(argc == 1) {
#ifndef JAPANESE
		printf("show help: \'help\'\n");
#else
		printf("ヘルプを表示するには\'help\'と入力してください\n");
select_mode:
		printf("*********** モードを選択してください *************\n");
		printf("* mode: 四則演算・関数電卓をモードで切り替えます *\n");
		printf("* line: すべての計算を一行で入力します           *\n");
		printf("**************************************************\n> \n");
#endif // JAPANESE
		fgets(buf, sizeof(buf), stdin);
		if(!strcmp(buf, "mode\n")) {
			printf("f: 関数モードに移行\n");
			printf("a: 四則演算モードに移行\n");
			printf("q: 終了\n");
			printf("はじめに数字を入力してから式を入力してください\n");
			printf("1<enter>+2<enter>*10<enter>のようにしてください\n");
			mode_calculate();
		} else if(!strcmp(buf, "line\n")) {
			while(!calc());
		} else {
			printf("もう一度入力してください\n");
			goto select_mode;
		}
	} else if(argc > 2) {
		calc_with_formula(argc, argv);
	} else {
	unknown_option:
		fprintf(stderr, "[%s]\n", argv[1]);
	}

	return 0;
}

/* calculator */

enum func_num {
	VALUE = 0,
	SIN,
	COS,
	TAN,
	ASIN,
	ACOS,
	ATAN,
	SINH,
	COSH,
	TANH,
	SQRT,
	EXP,
	LOG,
	LN,
	ABS,
	PLUS = '+',
	MINUS = '-',
	MUL = '*',
	DIV = '*',
	UNKNOWN = 100,
	NO_FUNC,
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

/* functions */
int    input_formula(char *, int);
int    to_RPN(char *);
double calc_RPN(void);
int    get_next_token(char *, int *, double *);
int    init_token_type(void);
int    append_variable_value(char *, double);
double get_variable_value(char *);
int    symbol_to_value(char *, int *, double *);
int    order(int);
int    print_style(double);

/* global variables */
char   token_type[0xff];
int    f_use_degree = 1; // default use degree (not use radian)
int    f_error = 0; // is error for calculate
int    f_was_calc = 1;
char   variable_table[100][100];
double variable_value[100];
int    variable_index;
char   variable_name[1024];

int set_angle_unit(int unit)
{
	if(unit == DEGREE)
		f_use_degree = 1;
	else
		f_use_degree = 0;
	return 0;
}

double radian_to_degree(double value)
{
	return (value * M_PI / 180.0);
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

int append_variable_value(char *name, double value)
{
	int i;

	for(i = 0; i < variable_index; i++) {
		if(!strcmp(variable_table[i], name)) {
			goto Label_already_used_name;
		}
	}
	i = variable_index;
	variable_index++;
	if(variable_index > 100) {
		//fprintf(stderr, "variable count too many\n");
		fprintf(stderr, "エラー: 変数名が多すぎます\n");
		return 0;
	}
	strcpy(variable_table[i], name);
Label_already_used_name:
	variable_value[i] = value;

	return 0;
}

double get_variable_value(char *name)
{
	double ret;
	int i;

	for(i = 0; i < variable_index; i++) {
		if(!strcmp(variable_table[i], name)) {
			ret = variable_value[i];
			return ret;
		}
	}
	//fprintf(stderr, "Error: unknown variable name [%s]\n", name);
	fprintf(stderr, "エラー: 定義されていない変数名です [%s]\n", name);
	return 0.0;
}

int symbol_to_value(char *str, int *ret, double *value)
{
	if(!strcmp(str, "quit")) {
		exit(0);
	} else if(!strcmp(str, "sin")) {
		*ret = SIN;
	} else if(!strcmp(str, "cos")) {
		*ret = COS;
	} else if(!strcmp(str, "tan")) {
		*ret = TAN;
	} else if(!strcmp(str, "asin")) {
		*ret = ASIN;
	} else if(!strcmp(str, "acos")) {
		*ret = ACOS;
	} else if(!strcmp(str, "atan")) {
		*ret = ATAN;
	} else if(!strcmp(str, "sinh")) {
		*ret = SINH;
	} else if(!strcmp(str, "cosh")) {
		*ret = COSH;
	} else if(!strcmp(str, "tanh")) {
		*ret = TANH;
	} else if(!strcmp(str, "sqrt")) {
		*ret = SQRT;
	} else if(!strcmp(str, "exp")) {
		*ret = EXP;
	} else if(!strcmp(str, "log")) {
		*ret = LOG;
	} else if(!strcmp(str, "ln")) {
		*ret = LN;
	} else if(!strcmp(str, "abs")) {
		*ret = ABS;
	} else if(!strcmp(str, "help") || !(strcmp(str, "?"))) {
		f_was_calc = 0;
		show_usage();
		*ret = UNKNOWN;
	} else if(!strcmp(str, "list")) {
		f_was_calc = 0;
		list_of_functions();
		*ret = UNKNOWN;
	} else if(!strcmp(str, "degree")) {
		f_was_calc = 0;
		set_angle_unit(DEGREE);
		//printf("degree mode\n");
		printf("Degreeを使います\n");
		*ret = UNKNOWN;
	} else if(!strcmp(str, "radian")) {
		f_was_calc = 0;
		set_angle_unit(RADIAN);
		//printf("radian mode\n");
		printf("Radianを使います\n");
		*ret = UNKNOWN;
	} else if(!strcmp(str, "pi") || !strcmp(str, "PI")) {
		*value = M_PI;
		return 2; // number
	} else if(!strncmp(str, "$", 1)) {
		*value = get_variable_value(str + 1);
		return 2; // number
	} else {
		f_was_calc = 0;
		*ret = UNKNOWN;
	}

	return 1; // symbol
}

int get_next_token(char *src, int *val, double *d_val)
{
	static int index;
	char c;
	char buf[1024];
	int value, ret, i;
	int type;
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
		type = symbol_to_value(buf, &value, &d_value);
		if(type == 1)
			ret = 4; // symbol
		else if(type == 2)
			ret = 1; // number
		break;
	default:
		//fprintf(stderr, "Error: Unknown charactor [%c]\n", c);
		fprintf(stderr, "エラー: 不明な文字 [%c]\n", c);
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
	token_type['.'] = NUMBER;

	for(i = 'a'; i <= 'z'; i++) {
		token_type[i] = SYMBOL;
	}

	for(i = 'A'; i <= 'Z'; i++) {
		token_type[i] = SYMBOL;
	}

	token_type['$'] = SYMBOL;
	token_type['_'] = SYMBOL;
	token_type['?'] = SYMBOL;

	token_type[' '] = SPACE;
	token_type['\n'] = SPACE;
	token_type['\r'] = SPACE;
	token_type['\t'] = SPACE;

	token_type['('] = SPLIT;
	token_type[')'] = SPLIT;
	token_type['*'] = OPERATOR;
	token_type['+'] = OPERATOR;
	token_type['-'] = OPERATOR;
	token_type['/'] = OPERATOR;
	token_type['^'] = OPERATOR;
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
	case ACOS:
	case ASIN:
	case ATAN:
	case SINH:
	case COSH:
	case TANH:
	case SQRT:
	case EXP:
	case LOG:
	case LN:
	case ABS:
		return 5;
	case '^':
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
	int i;
	int f;
	int top;
	int type;
	int value;
	double d_value;

	init_stack();

	strncpy(buf0, src, sizeof(buf0));
	if(!strncmp(src, "$", 1)) {
		f = 0;
		for(i = 1; src[i] != '\0'; i++) {
			if(src[i] == '=') f = 1;
			if(f == 0) variable_name[i - 1] = src[i];
			if(src[i] == '=') {
				strncpy(buf0, src + i + 1, sizeof(buf0));
				break;
			}
		}
		if(f == 0) {
			variable_name[0] = '\0';
		} else {
			variable_name[i - 1] = '\0';
		}
	}
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
					//fprintf(stderr, "Error: Invalid syntax\n");
					fprintf(stderr, "エラー: 無効な書式です\n");
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
					//fprintf(stderr, "Error: Syntax error\n");
					fprintf(stderr, "エラー: 無効な書式です\n");
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
			//fprintf(stderr, "Error: Invalid syntax\n");
			fprintf(stderr, "エラー: 無効な書式です\n");
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

	f_error = 0;
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
					//fprintf(stderr, "Error: Division error\nDivision by zero\n");
					fprintf(stderr, "エラー: ゼロで割ることはできません [%f / %f]\n", i, j);
					f_error = 1;
					return -1;
				}
				stack2[index++] = i / j;
				break;
			case '^':
				j = stack2[--index];
				i = stack2[--index];
				stack2[index++] = pow(i, j);
				break;
			case SIN:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = sin(i);
				break;
			case COS:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = cos(i);
				break;
			case TAN:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				if(i == (M_PI / 2)) {
					//fprintf(stderr, "Error: tan90(deg) not calculate!!\n");
					fprintf(stderr, "エラー: タンジェント90度は計算できません\n");
					f_was_calc = 0;
					break;
				}
				stack2[index++] = tan(i);
				break;
			case ASIN:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = asin(i);
				break;
			case ACOS:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = acos(i);
				break;
			case ATAN:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = atan(i);
				break;
			case SINH:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = sinh(i);
				break;
			case COSH:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = cosh(i);
				break;
			case TANH:
				i = stack2[--index];
				if(f_use_degree)  i = radian_to_degree(i);
				stack2[index++] = tanh(i);
				break;
			case SQRT:
				i = stack2[--index];
				if(i < 0) {
					//fprintf(stderr, "Error: minus number is not calculate square root\n");
					fprintf(stderr, "エラー: マイナスの数の平方根は計算できません\n");
					f_was_calc = 0;
					break;
				}
				stack2[index++] = sqrt(i);
				break;
			case EXP:
				i = stack2[--index];
				stack2[index++] = exp(i);
				break;
			case LOG:
				i = stack2[--index];
				if(i < 0) {
					//fprintf(stderr, "Error: minus number is not calculate common logarithm\n");
					fprintf(stderr, "エラー: マイナスの数の常用対数は計算できません\n");
					f_was_calc = 0;
					break;
				}
				stack2[index++] = log10(i);
				break;
			case LN:
				i = stack2[--index];
				if(i < 0) {
					//fprintf(stderr, "Error: minus number is not calculate natural logarithm\n");
					fprintf(stderr, "エラー: マイナスの数の自然対数は計算できません\n");
					f_was_calc = 0;
					break;
				}
				stack2[index++] = log(i);
				break;
			case ABS:
				i = stack2[--index];
				stack2[index++] = abs(i);
				break;
			case NO_FUNC:
				break;
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
	f_was_calc = 1;
	printf("> "); /* prompt */
	if(input_formula(buf, sizeof(buf)) == -1) return -1;
	if(to_RPN(buf) == -1) return -1;
	ret = calc_RPN();
	if(f_error) return 0;
	append_variable_value("prev", ret);
	if(variable_name[0] != '\0') {
		append_variable_value(variable_name, ret);
	}
	/* real number or natural number */
	if(f_was_calc)
		print_style(ret);

	return 0;
}

int calc_with_formula(int count, char *formula_data[])
{
	char buf[1024];
	double ret;
	int i;
	
	buf[0] = '\0';

	init_token_type();
	for(i = 1; i < count; i++) {
		strncat(buf, formula_data[i], sizeof(buf));
	}
	if(to_RPN(buf) == -1) return -1;
	ret = calc_RPN();
	if(f_error) return 0;
	/* real number or natural number */
	print_style(ret);

	return 0;
}
	
int show_usage(void)
{
#ifndef JAPANESE
	printf("This command is calculator with one line formula.\n");
	printf("Input Example:\n");
	printf("\t10 + 5\n");
	printf("\t10+5\n");
	printf("\t$abc = 3*(4+6)-7\n");
	printf("\t$abc + sin(45+30) + cos45 + tan(90 / (4 - 2))\n");
	printf("\t10.5*sin(2.5*100)\n");
	printf("Exit ways:\n");
	printf("\tEnter 'quit' or 'quit()'\n");
	printf("Help:\n");
	printf("\thelp: show this message\n");
	printf("\tlist: list of function\n");
#else
	printf(
		"一行ごとに計算するコマンドです\n"
		"四則演算・各種関数・カッコ・変数が使えます\n"
		"変数は$(ドルマーク)を先頭につけた文字列です\n"
		"直前の計算結果は変数$prevを利用することで参照できます\n"
		"変数に値を代入するときは=(イコール)を用いてください\n"
		"入力の例:\n"
		"\t10 + 5\n"
		"\t10+5\n"
		"\t$abc = 3*(4+6)-7\n"
		"\t$abc + sin(45+30) + cos45 + tan(90 / (4 - 2))\n"
		"\t10.5*sin(2.5*100)\n"
		"終了のしかた:\n"
		"\t\'quit\'と入力してください\n"
	);
#endif // JAPANESE
	return 0;
}

int list_of_functions(void)
{
#ifndef JAPANESE
	printf(
		"trigonometric function:\n"
		"\tsin: sine\n"
		"\tcos: cosine\n"
		"\ttan: tangent\n"
		"inverse trigonometric function:\n"
		"\tasin: inverse sine\n"
		"\tacos: inverse cosine\n"
		"\tatan: inverse tangent\n"
		"hyperbolic function:\n"
		"\tsinh: hyperbolic sine\n"
		"\tcosh: hyperbolic cosine\n"
		"\ttanh: hyperbolic tanjent\n"
		"\n"
		"\tsqrt: square root\n"
		"\texp: exponential function\n"
		"\tlog: common logarithm\n"
		"\tln: natural logarithm\n"
		"\tabs: absolute value\n"
	);
#else
	printf(
		"三角関数:\n"
		"\tsin: サイン\n"
		"\tcos: コサイン\n"
		"\ttan: タンジェント\n"
		"逆三角関数:\n"
		"\tasin: アークサイン\n"
		"\tacos: アークコサイン\n"
		"\tatan: アークタンジェント\n"
		"双曲線関数:\n"
		"\tsinh: ハイパボリックサイン\n"
		"\tcosh: ハイパボリックコサイン\n"
		"\ttanh: ハイパボリックタンジェント\n"
		"その他の関数:\n"
		"\tsqrt: 平方根\n"
		"\texp: 指数関数\n"
		"\tlog: 常用対数\n"
		"\tln: 自然対数\n"
		"\tabs: 絶対値\n"
	);
#endif // JAPANESE
	return 0;
}

int print_style(double value)
{
	int i;
	char buf[1024];

	sprintf(buf, "%lf", value);
	for(i = strlen(buf) - 1; buf + i > buf; i--) {
		if(buf[i] == '.') {
			buf[i] = '\0';
			break;
		}
		if(buf[i] == '0')
			buf[i] = '\0';
		else
			break;
	}
	printf("%s\n", buf);

	return 0;
}

/* formula data */

struct t_formula {
	enum {
		e_OPERATOR = 0,
		e_VALUE
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
	int ret;
	if(start_formula == NULL) return -1;
	*data = start_formula->data;
	ret = start_formula->type;
	return ret;
}

/* stack data */

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
	if(!p) return -1; /* memory allocate error */
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

/* with mode */
enum e_mode {
	ARITHMETIC,
	FUNCTION,
} Mode;

enum e_calc {
	CALC,
	NO_CALC,
} Calc;

int mode_calculate(void)
{
	int mode = ARITHMETIC;
	int flag = CALC;
	double f, g;
	char c, func;

	printf("数式を入力:\n");
	scanf("%lf", &f);
	for(;;) {
		scanf(" %c", &c);
		if(c == 'q') break;
		if(mode == ARITHMETIC) {
			if(c == 'f') {
				mode = FUNCTION;
				continue;
			}
			scanf("%lf", &g);
			switch(c) {
			case '+':
				f = f + g;
				break;
			case '-':
				f = f - g;
				break;
			case '*':
				f = f * g;
				break;
			case '/':
				if(g != 0)
					f = f / g;
				break;
			default:
				break;
			}
		} else if(mode == FUNCTION) {
			if(c == 'a') {
				mode = ARITHMETIC;
				continue;
			}
			scanf(" %c", &func);
			scanf("%lf", &g);
			switch(func) {
			case 's':
				g = sin(radian_to_degree(g));
				break;
			case 'c':
				g = cos(radian_to_degree(g));
				break;
			case 't':
				g = tan(radian_to_degree(g));
				break;
			default:
				break;
			}
			switch(c) {
			case '+':
				f = f + g;
				break;
			case '-':
				f = f - g;
				break;
			case '*':
				f = f * g;
				break;
			case '/':
				if(g != 0)
					f = f / g;
				break;
			default:
				break;
			}
		}
		if(flag == CALC)
			print_style(f);
	}
	return 0;
}

