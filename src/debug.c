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

void print_startup_msg(void) {

	printf("\n\n");
	printf("*************************************************************************\n");
	printf("*                                                                       *\n");
	printf("*"ANSI_COLOR_RED"    $$$$$$$\\   $$$$$$\\   $$$$$$\\  $$\\      $$\\  $$$$$$\\  $$\\   $$\\     "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_RED"    $$  __$$\\ $$  __$$\\ $$  __$$\\ $$$\\    $$$ |$$  __$$\\ $$$\\  $$ |    "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_RED"    $$ |  $$ |$$ /  $$ |$$ /  \\__|$$$$\\  $$$$ |$$ /  $$ |$$$$\\ $$ |    "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_RED"    $$$$$$$  |$$$$$$$$ |$$ |      $$\\$$\\$$ $$ |$$$$$$$$ |$$ $$\\$$ |    "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_RED"    $$  ____/ $$  __$$ |$$ |      $$ \\$$$  $$ |$$  __$$ |$$ \\$$$$ |    "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_RED"    $$ |      $$ |  $$ |$$ |  $$\\ $$ |\\$  /$$ |$$ |  $$ |$$ |\\$$$ |    "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_RED"    $$ |      $$ |  $$ |\\$$$$$$  |$$ | \\_/ $$ |$$ |  $$ |$$ | \\$$ |    "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_RED"    \\__|      \\__|  \\__| \\______/ \\__|     \\__|\\__|  \\__|\\__|  \\__|    "ANSI_COLOR_RESET"*\n");
	printf("*                                                                       *\n");
	printf("*"ANSI_COLOR_GREEN"                     __  _________    __ __         __                "ANSI_COLOR_RESET" *\n");
	printf("*"ANSI_COLOR_GREEN"                    / / / / __/ _ )  / // /__  ___ / /_               "ANSI_COLOR_RESET" *\n");
	printf("*"ANSI_COLOR_GREEN"                   / /_/ /\\ \\/ _  | / _  / _ \\(_-</ __/               "ANSI_COLOR_RESET" *\n");
	printf("*"ANSI_COLOR_GREEN"                   \\____/___/____/ /_//_/\\___/___/\\__/                "ANSI_COLOR_RESET" *\n");
	printf("*                                                                       *\n");
	printf("*"ANSI_COLOR_YELLOW"                   *** PAC MAN ***                                     "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"               *PACMANPACMANPACMANPAC*                                 "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"           *MANPACMANPACMANPACMANPACMANPA*                             "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"        *CMANPACMANPACMANPACMANPACMANPACMANP*                          "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"      *ACMANPACMANPACMANPACMANPACMANPACMANPACM*                        "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"    *ANPACMANPACMANPACMANPACMANPACMANPACMA*                            "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"   *NPACMANPACMANPACMANPACMANPACMANPACM*                               "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"  *ANPACMANPACMANPACMANPACMANPACMAN*                                   "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW" *PACMANPACMANPACMANPACMANPACM*                                        "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW" *ANPACMANPACMANPACMANPACM*                DIG           DIG           "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW" *ANPACMANPACMANPACMANP*                  DIGIT         DIGIT          "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW" *ACMANPACMANPACMANPACMANP*                IT*           IT*           "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW" *ACMANPACMANPACMANPACMANPACMA*                                        "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"  *NPACMANPACMANPACMANPACMANPACMANP*                                   "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"   *ACMANPACMANPACMANPACMANPACMANPACMAN*                               "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"    *PACMANPACMANPACMANPACMANPACMANPACMANP*                            "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"      *ACMANPACMANPACMANPACMANPACMANPACMANPACM*                        "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"        *ANPACMANPACMANPACMANPACMANPACMANPAC*                          "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"           *MANPACMANPACMANPACMANPACMANPA*                             "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"               *CMANPACMANPACMANPACMA*                                 "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"                   *** PAC MAN ***                                     "ANSI_COLOR_RESET"*\n");
	printf("*"ANSI_COLOR_YELLOW"                                                                       "ANSI_COLOR_RESET"*\n");
	printf("*************************************************************************\n");
	#ifdef DEBUG
	printf("*                                DEBUG MODE                             *\n");
	printf("*************************************************************************\n");
	#endif                                    
}
void print_help_string(void) {
	printf("\n");
	colorprint("Available commands: ", MAGENTA);
	printf("connect [mcu|fpga]       --  Connect to <device>, defaults to both devices.\n");
	printf("run [mcu|fpga]           --  Start normal transfers to <device>, defaults to both devices.\n");
	printf("stop [mcu|fgpa]          --  Stop all transactions with <device>, defaults to both devices.\n");
	printf("send <mcu|fpga> [n]      --  Send N message to <device>, defaults to 1.\n");
	printf("recv <mcu|fgpa> [n]      --  Set up receive of N message from <device>, defaults to 1.\n");
	printf("sendrecv <mcu|fpga> [n]  --  Send and set up receive of N message to/from <device>, defaults to 1.\n");
	printf("quit, exit               --  Quit the program\n");
	printf("help                     --  Print list of available commands\n");
	printf("\n");
}