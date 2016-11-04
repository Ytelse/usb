#include "cmd_parser.h"
#include "debug.h"

cmd_t parse_cmd(char* string) {
	cmd_t cmd = INVALID_CMD;	

	/* TODO: Update with planned commands corresponding to cmd_t types */

	// if (strcmp(stringBuffer, "testsend") == 0) {
	// 	cmd = TESTSEND;
	// } else if (strcmp(stringBuffer, "ts") == 0) {
	// 	cmd = TESTSEND;
	// } else if ((strcmp(stringBuffer, "testsend10") == 0)) {
	// 	cmd = TESTSEND10;
	// } else if ((strcmp(stringBuffer, "ts10") == 0)) {
	// 	cmd = TESTSEND10;
	// } else if (strcmp(stringBuffer, "testrecv") == 0) {
	// 	cmd = TESTRECV;
	// } else if (strcmp(stringBuffer, "tr") == 0) {
	// 	cmd = TESTRECV;
	// } else if (strcmp(stringBuffer, "testrecv10") == 0) {
	// 	cmd = TESTRECV10;
	// } else if (strcmp(stringBuffer, "tr10") == 0) {
	// 	cmd = TESTRECV10;
	// } else if (strcmp(stringBuffer, "testsendrecv") == 0) {
	// 	cmd = TESTSENDRECV;
	// } else if (strcmp(stringBuffer, "tsr") == 0) {
	// 	cmd = TESTSENDRECV;
	// } else if (strcmp(stringBuffer, "help") == 0) {
	// 	cmd = HELP;
	// } else if (strcmp(stringBuffer, "quit") == 0) {
	// 	cmd = QUIT;
	// } else  if (strcmp(stringBuffer, "exit") == 0) {
	// 	cmd = QUIT;
	// } else {
	// 	/* Do nothing */
	// }

	return cmd;
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