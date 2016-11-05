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

	/* TODO: Handle 'connect <target>' where target can be <none> (meaning both), mcu or fpga */
	/* TODO: Handle 'run <target>'	   -- 							*						 */
	/* TODO: Handle 'stop <target>'    --							*						 */
	/* TODO: Handle 'help' */
	/* TODO: Handle 'quit' */
	/* TODO: Handle 'send <target> n'     where target can be mcu or fpga */
	/* TODO: Handle 'recv <target> n'     --				*		   -- */
	/* TODO: Handle 'sendrecv <target> n' --				*		   -- */

	/* Handle the easy cases first */

	if (strcmp(string, "help") == 0) {
		cmd.command = HELP;
	} else if (strcmp(string, "quit") == 0) {
		cmd.command = QUIT;
	} else  if (strcmp(string, "exit") == 0) {
		cmd.command = QUIT;
	} else {
		char* saveptr; /* Pointer required by strtok_r */
		char *_cmd, *_target, *_amount; /* Pointers to the string tokens we want */

		_cmd = strtok_r(string, " ", &saveptr); /* Get first token from 'string' */

		if (_cmd != NULL) {
			_target = strtok_r(NULL, " ", &saveptr); /* Pass NULL to get next token from 'string' */
			
			if (_target != NULL) { /* We have a specific target for the command */
				if (strcmp(_cmd, "run") == 0) {
					cmd.command = RUN;
					if (strcmp(_target, "mcu") == 0) {
						cmd.target = YTELSE_MCU_DEVICE;
					} else {
						cmd.target = YTELSE_FPGA_DEVICE;
					}
				} else if (strcmp(_cmd, "stop") == 0) {
					cmd.command = STOP;
					if (strcmp(_target, "mcu") == 0) {
						cmd.target = YTELSE_MCU_DEVICE;
					} else {
						cmd.target = YTELSE_FPGA_DEVICE;
					}
				} else if (strcmp(_cmd, "connect") == 0) {
					cmd.command = CONNECT;
					if (strcmp(_target, "mcu") == 0) {
						cmd.target = YTELSE_MCU_DEVICE;
					} else {
						cmd.target = YTELSE_FPGA_DEVICE;
					}
				} else { /* Command is one of send / recv / sendrecv, and we need to get amount as well */
					_amount = strtok_r(NULL, " ", &saveptr);
					if (_amount != NULL) {
						cmd.N = strtol(_amount, NULL, 10);
					} else {
						cmd.N = 1;
					}

					if (strcmp(_cmd, "send") == 0) {
							cmd.command = TESTSEND;
							if (strcmp(_target, "mcu") == 0) {
								cmd.target = YTELSE_MCU_DEVICE;
							} else {
								cmd.target = YTELSE_FPGA_DEVICE;
							}
						} else if (strcmp(_cmd, "recv") == 0) {
							cmd.command = TESTRECV;
							if (strcmp(_target, "mcu") == 0) {
								cmd.target = YTELSE_MCU_DEVICE;
							} else {
								cmd.target = YTELSE_FPGA_DEVICE;
							}
						} else if (strcmp(_cmd, "sendrecv") == 0) {
							cmd.command = TESTSENDRECV;
							if (strcmp(_target, "mcu") == 0) {
								cmd.target = YTELSE_MCU_DEVICE;
							} else {
								cmd.target = YTELSE_FPGA_DEVICE;
							}
						} else {
							cmd.command = INVALID_CMD;
							cmd.target = YTELSE_NO_DEVICE;
							cmd.N = -1;
						}/* If none of these, it's an invalid command */
				} 
			} else { /* If no target, a valid command refers to both devices */
				cmd.target = YTELSE_BOTH_DEVICES;
				if (strcmp(_cmd, "run") == 0) {
					cmd.command = RUN;
				} else if (strcmp(_cmd, "stop") == 0) {	
					cmd.command = STOP;
				} else if (strcmp(_cmd, "connect") == 0) {
					cmd.command = CONNECT;
				} else {
					cmd.command = INVALID_CMD;
					cmd.target = YTELSE_NO_DEVICE;
					cmd.N = -1;
				}/* If not one of these, just return INVALID_CMD */
			}
		}
	}

	return cmd;
}

void print_help_string(void) {

	/* TODO: Update available commands */

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