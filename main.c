
#include <stdio.h>
#include <string.h>

#include "calc.h"

int main(int argc, char *argv[])
{
	if(argc == 2) {
		if(!strcmp(argv[1], "-h") ||
		   !strcmp(argv[1], "--help")) {
			show_usage();
			return 0;
		}
	}
	show_usage();
	while(!calc()) {
		;
	}

	return 0;
}

