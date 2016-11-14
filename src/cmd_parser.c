#include "defs.h"
#include "cmd_parser.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

pacman_command_t parse_cmd(char* string) {
	
	pacman_command_t cmd;	

	cmd.command = INVALID_CMD;
	cmd.target = PACMAN_NO_DEVICE;
	/* Handle the easy cases first */

	if (strcmp(string, "help") == 0) {
		cmd.command = HELP;
	} else if (strcmp(string, "art") == 0) {
		cmd.command = ART;
	} else if (strcmp(string, "quit") == 0) {
		cmd.command = QUIT;
	} else  if (strcmp(string, "exit") == 0) {
		cmd.command = QUIT;
	} else {
		char* saveptr; /* Pointer required by strtok_r */
		char *_cmd, *_target; /* Pointers to the string tokens we want */

		_cmd = strtok_r(string, " ", &saveptr); /* Get first space seperated token from 'string' */

		if (_cmd != NULL) {
			_target = strtok_r(NULL, " ", &saveptr); /* Pass NULL to get next token from 'string' */
			
			if (_target != NULL) { /* We have a specific target for the command */
				if (strcmp(_cmd, "run") == 0) {
					cmd.command = RUN;
					if (strcmp(_target, "mcu") == 0) {
						cmd.target = PACMAN_MCU_DEVICE;
					} else {
						cmd.target = PACMAN_FPGA_DEVICE;
					}
				} else if (strcmp(_cmd, "stop") == 0) {
					cmd.command = STOP;
					if (strcmp(_target, "mcu") == 0) {
						cmd.target = PACMAN_MCU_DEVICE;
					} else {
						cmd.target = PACMAN_FPGA_DEVICE;
					}
				} else if (strcmp(_cmd, "connect") == 0) {
					cmd.command = CONNECT;
					if (strcmp(_target, "mcu") == 0) {
						cmd.target = PACMAN_MCU_DEVICE;
					} else {
						cmd.target = PACMAN_FPGA_DEVICE;
					}
				} else if (strcmp(_cmd, "test") == 0) {
					cmd.command = TEST_CONNECTION;
					if (strcmp(_target, "mcu") == 0) {
						cmd.target = PACMAN_MCU_DEVICE;
					} else {
						cmd.target = PACMAN_FPGA_DEVICE;
					}
				} 
			} else { /* If no target, a valid command refers to both devices */
				cmd.target = PACMAN_BOTH_DEVICES;
				if (strcmp(_cmd, "run") == 0) {
					cmd.command = RUN;
				} else if (strcmp(_cmd, "stop") == 0) {	
					cmd.command = STOP;
				} else if (strcmp(_cmd, "connect") == 0) {
					cmd.command = CONNECT;
				} else if (strcmp(_cmd, "test") == 0) {
					cmd.command = TEST_CONNECTION;
				} else {
					cmd.command = INVALID_CMD;
					cmd.target = PACMAN_NO_DEVICE;
				}/* If not one of these, just return INVALID_CMD */
			}
		}
	}

	return cmd;
}