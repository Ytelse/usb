#include "cmd_parser.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

cmd_t parse_cmd(char* string) {
	cmd_t cmd = INVALID_CMD;	

	/* TODO: Update with planned commands corresponding to cmd_t types */
	/* TODO: General commands */
	/* TODO: Check for 'connect <target>' where target can be <none> (meaning both), mcu or fpga */
	/* TODO: Check for 'run <target>'	  -- 							*						 */
	/* TODO: Check for 'stop <target>'    --							*						 */
	/* TODO: Check for 'help' */
	/* TODO: Check for 'quit' */
	/* TODO: Test commands */
	/* TODO: Check for 'send n <target>' where target can be mcu or fpga */
	/* TODO: Check for 'recv n <target>' --				*			  -- */
	/* TODO: Check for 'sendrecv n <target>' --			*			  -- */


	if (strcmp(string, "help") == 0) {
		cmd = HELP;
	} else if (strcmp(string, "quit") == 0) {
		cmd = QUIT;
	} else  if (strcmp(string, "exit") == 0) {
		cmd = QUIT;
	} else if (strcmp(string, "connect") == 0) {
		cmd = CONNECT_MCU;
	} else {
		/* TODO: Check space seperated strings */
	}

	return cmd;
}

/* Get the number of messages from space separated string input */
int splitOnSpaceGetLast(char* stringBuffer) {
	strsep((char**)&stringBuffer, " ");
	char* token = strsep((char**)&stringBuffer, " ");
	int number = atoi(token);
	return number;
}

void print_help_string(void) {

	/* TODO: Update available commands */

	printf("\n");
	colorprint("Available commands: ", MAGENTA);
	printf("connect mcu|fpga    --  Connect to <device>");
	printf("testsend, ts        --  Send 1 message to MCU\n");
	printf("testrecv, tr        --  Set up receive of 1 message from MCU\n");
	printf("testsend10, ts10    --  Send 10 messages to MCU\n");
	printf("testrecv10, tr10    --  Set up receive of 10 messages from MCU\n");
	printf("testsendrecv, tsr   --  Send and set up receive of 1 message to/from MCU\n");
	printf("quit, exit          --  Quit the program\n");
	printf("help                --  Print list of available commands\n");
	printf("\n");
}