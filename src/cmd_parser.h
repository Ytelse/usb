#ifndef __CMD_PARSER_H_
#define __CMD_PARSER_H_

typedef enum commands {
	/* Useful commands */
	INVALID_CMD, 		/* No command selected */
	CONNECT,			/* Connect to both devices */
	CONNECT_MCU,		/* Connect to MCU */
	CONNECT_FPGA,		/* Connect to FPGA */
	RUN,				/* Run full program */
	STOP,				/* Stop full program */
	RUN_FPGA,			/* Only send data to FPGA */
	RUN_MCU,			/* Only receive from MCU */
	STOP_FPGA,			/* Stop whatever transactions are going on between host and FPGA */
	STOP_MCU,			/* Stop whatever transactions are going on between host and MCU */
	HELP, 				/* Print available commands */
	QUIT,				/* Quit the program */
	/* Test commands */
	MCU_TESTSEND_N,		/* Send N messages to MCU */
	FPGA_TESTSEND_N,	/* Send N messages to FPGA */
	MCU_TESTRECV,		/* Set up receive of 1 message from MCU */
	FPGA_TESTRECV,		/* Set up receive of 1 message from FPGA */
	MCU_TESTSENDRECV,	/* Send and set up receive of 1 message to/from MCU */
	FPGA_TESTSENDRECV	/* Send and set up receive of 1 message to/from FPGA */
} cmd_t;


cmd_t parse_cmd(char* string);

/* Get the number of messages from space separated string input */
int splitOnSpaceGetLast(char* stringBuffer);

/* Print list of available commands */
void print_help_string(void);


#endif /* __CMD_PARSE_H_ */

