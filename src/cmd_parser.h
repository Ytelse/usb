#ifndef __CMD_PARSER_H_
#define __CMD_PARSER_H_

#include "ytelse_comm.h"

typedef enum commands {
	/* Useful commands */
	INVALID_CMD, 		/* No command selected */
	CONNECT,			/* Connect to specified device(s) */
	RUN,				/* Run full program specified by device */
	STOP,				/* Stop whatever transactions are going on between host and given device */
	HELP, 				/* Print available commands */
	QUIT,				/* Quit the program */
	/* Test commands */
	TESTSEND,		/* Send N messages to specified device */
	TESTRECV,		/* Set up receive of N message from specified device */
	TESTSENDRECV,	/* Send and set up receive of N message to/from specified device */
} cmd_t;

typedef struct Command {
	cmd_t command;
	ytelse_device_t target;
	int N;
} ytelse_command_t;


ytelse_command_t parse_cmd(char* string);

/* Get the number of messages from space separated string input */
// int splitOnSpaceGetLast(char* stringBuffer);

/* Print list of available commands */
void print_help_string(void);


#endif /* __CMD_PARSE_H_ */

