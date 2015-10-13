
#include <stdio.h>
#include <string.h>

#include "calc.h"

int main(int argc, char *argv[])
{
	if(argc == 2) {
		if(!strcmp(argv[1], "-h") ||
		   !strcmp(argv[1], "--help")) {
			/* help option */
			show_usage();
			return 0;
		} else if(!strcmp(argv[1], "-l") ||
		          !strcmp(argv[1], "--list")) {
			list_of_functions();
		} else if(!strcmp(argv[1], "-r") ||
		          !strcmp(argv[1], "--radian")) {
			set_angle_unit(RADIAN);
			goto calculate;
		} else if(!strcmp(argv[1], "-d") ||
		          !strcmp(argv[1], "--degree")) {
			set_angle_unit(DEGREE);
			goto calculate;
		} else {
			/* unknown option */
			goto unknown_option;
		}
	} else if(argc == 1) {
calculate:
		show_usage();
		while(!calc()) ;
	} else if(argc > 2) {
		calc_with_formula(argc, argv);
	} else {
unknown_option:
		fprintf(stderr, "Error: Unknown option [%s]\n", argv[1]);
	}

	return 0;
}

