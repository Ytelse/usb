#include "cmd_parser.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

ytelse_command_t parse_cmd(char* string) {
	ytelse_command_t cmd;	

	cmd.command = INVALID_CMD;
	cmd.target = YTELSE_NO_DEVICE;
	cmd.N = -1;

	/* TODO: Update with planned commands corresponding to cmd_t types */
	/* TODO: General commands */
	/* TODO: Check for 'connect <target>' where target can be <none> (meaning both), mcu or fpga */
	/* TODO: Check for 'run <target>'	  -- 							*						 */
	/* TODO: Check for 'stop <target>'    --							*						 */
	/* TODO: Check for 'help' */
	/* TODO: Check for 'quit' */
	/* TODO: Test commands */
	/* TODO: Check for 'send <target> n' where target can be mcu or fpga */
	/* TODO: Check for 'recv <target> n' --				*			  -- */
	/* TODO: Check for 'sendrecv <target> n' --			*			  -- */


	if (strcmp(string, "help") == 0) {
		cmd.command = HELP;
	} else if (strcmp(string, "quit") == 0) {
		cmd.command = QUIT;
	} else  if (strcmp(string, "exit") == 0) {
		cmd.command = QUIT;
	} else {
		/* TODO: Check space seperated strings */
		char* saveptr; /* Pointer required by strtok_r */
		char *_cmd, *_target, *_amount; /* Pointers to the string tokens we want */

		_cmd = strtok_r(string, " ", &saveptr); /* Get first token from 'string' */

		if (_cmd != NULL) {
			_target = strtok_r(NULL, " ", &saveptr); /* Pass NULL to get next token from 'string' */
			
			if (_target != NULL) { /* We have a specific target for the command */
				if (strcmp(_cmd, "run") == 0) {
					if (strcmp(_target, "mcu") == 0) {
						colorprint("TODO: Run mcu", MAGENTA);
					} else {
						colorprint("TODO: Run fpga", MAGENTA);
					}
				} else if (strcmp(_cmd, "stop") == 0) {
					if (strcmp(_target, "mcu") == 0) {
						colorprint("TODO: Stop mcu", MAGENTA);
					} else {
						colorprint("TODO: Stop fpga", MAGENTA);
					}
				} else if (strcmp(_cmd, "connect") == 0) {
					if (strcmp(_target, "mcu") == 0) {
						colorprint("TODO: Connect mcu", MAGENTA);
					} else {
						colorprint("TODO: Connect fpga", MAGENTA);
					}
				} else { /* Command is one of send / recv / sendrecv, and we need to get amount as well */
					_amount = strtok_r(NULL, " ", &saveptr);
					if (_amount != NULL) {
						if (strcmp(_cmd, "send") == 0) {
							if (strcmp(_target, "mcu") == 0) {
								colorprint("TODO: Send N packets to mcu", MAGENTA);
							} else {
								colorprint("TODO: Send N packets to fpga", MAGENTA);
							}
						} else if (strcmp(_cmd, "recv") == 0) {
							if (strcmp(_target, "mcu") == 0) {
								colorprint("TODO: Receive N packets from mcu", MAGENTA);
							} else {
								colorprint("TODO: Receive N packets from fpga", MAGENTA);
							}
						} else if (strcmp(_cmd, "sendrecv") == 0) {
							if (strcmp(_target, "mcu") == 0) {
								colorprint("TODO: Send and receive N packets to/from mcu", MAGENTA);
							} else {
								colorprint("TODO: Send and receive N packets to/from fpga", MAGENTA);
							}
						} /* If none of these, it's an invalid command */
					} else {
						/* If no amount given, default to 1 */
						if (strcmp(_cmd, "send") == 0) {
							if (strcmp(_target, "mcu") == 0) {
								colorprint("TODO: Send 1 packet to mcu", MAGENTA);
							} else {
								colorprint("TODO: Send 1 packet to fpga", MAGENTA);
							}
						} else if (strcmp(_cmd, "recv") == 0) {
							if (strcmp(_target, "mcu") == 0) {
								colorprint("TODO: Receive 1 packet from mcu", MAGENTA);
							} else {
								colorprint("TODO: Receive 1 packet from fpga", MAGENTA);
							}
						} else if (strcmp(_cmd, "sendrecv") == 0) {
							if (strcmp(_target, "mcu") == 0) {
								colorprint("TODO: Send and receive 1 packet to/from mcu", MAGENTA);
							} else {
								colorprint("TODO: Send and receive 1 packet to/from fpga", MAGENTA);
							}
						} /* If none of these, it's an invalid command */
					}
				} 
			} else { /* If no target, a valid command refers to both devices */
				
				if (strcmp(_cmd, "run") == 0) {
					colorprint("TODO: Run both devices", MAGENTA);
				} else if (strcmp(_cmd, "stop") == 0) {	
					colorprint("TODO: Stop both devices", MAGENTA);
				} else if (strcmp(_cmd, "connect") == 0) {
					colorprint("TODO: Connect both devices", MAGENTA);
				} /* If not one of these, just return INVALID_CMD */
			}
		}
	}

	return cmd;
}

// /* Get the number of messages from space separated string input */
// static int splitOnSpaceGetLast(char* stringBuffer) {
// 	strsep((char**)&stringBuffer, " ");
// 	char* token = strsep((char**)&stringBuffer, " ");
// 	int number = atoi(token);
// 	return number;
// }

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