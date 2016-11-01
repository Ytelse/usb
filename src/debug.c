#include "debug.h"

#include <stdio.h>

void debugprint(char* string, int color) {
	#ifdef DEBUG
	switch (color) {
		case RED :
			printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case GREEN :
			printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case YELLOW :
			printf(ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case BLUE :
			printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case MAGENTA :
			printf(ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case CYAN :
			printf(ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", string);
			break;
		default :
			printf("%s\n", string);
	}
	#endif
}

void colorprint(char* string, int color) {
	switch (color) {
		case RED :
			printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case GREEN :
			printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case YELLOW :
			printf(ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case BLUE :
			printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case MAGENTA :
			printf(ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET "\n", string);
			break;
		case CYAN :
			printf(ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", string);
			break;
		default :
			printf("%s\n", string);
	}
}