#include "debug.h"
#include "ytelse_comm.h"
#include "cmd_parser.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include <ctype.h>
#include <signal.h>

/* TODO: Rework this implementation? */
static int number_of_messages_to_send = 0;

/* enum for main loop flow control */
typedef enum MainloopState {
	/* Overall */
	INIT,							/* Initial state */
	FINALIZE, 						/* Exit state, stop all communication */
	CONNECTING,						/* Connect to one or more devices */
	RUNNING,						/* Host is sending data to FPGA and receiving results from MCU */
	STOPPING,						/* Halt the program without releasing devices and quitting */
	TESTING,						/* Running some test function for a device */
	GET_CMD,						/* Waiting for command input */
	UNDEFINED						/* Undefined state */
} main_state_t;

typedef enum USBState {
	DISCONNECTED,					/* No USB devices connected to host */
	CONNECTED,						/* Both USB devices connected to host */
	CONNECTED_FPGA,					/* Only FPGA connected to host */
	CONNECTED_MCU,					/* Only MCU connected to host */
	CONNECTING_ALL,					/* Attempting to connect to both devices */
	CONNECTING_FPGA,				/* Attempting to connect to FPGA */
	CONNECTING_MCU,					/* Attempting to connect to MCU */
} usb_state_t;

typedef enum Tests {
	/* General tests */
	TESTSEND_N_TEST,
	/* MCU tests, add more as needed */
	MCU_TESTSEND_TEST,
	MCU_TESTSEND10_TEST,
	MCU_TESTRECV_TEST,
	MCU_TESTRECV10_TEST,
	MCU_TESTSENDRECV_TEST,
	/* FPGA tests, currently just placeholders */
	FPGA_TESTSEND_TEST,
	FPGA_TESTSEND10_TEST,
	FPGA_TESTRECV_TEST,
	FPGA_TESTRECV10_TEST,
	FPGA_TESTSENDRECV_TEST,
	/* Initial value */
	NO_TEST
} test_t;

typedef struct State {
	main_state_t main_state; /* State of mainloop */
	usb_state_t usb_state;
	cmd_t cmd;
	test_t test;
} state_t;

/* Catch ctrl-c */

volatile sig_atomic_t _kill;

void inthand(int signum) {
    _kill = 1;
}

/* Function prototypes */
void mainloop(libusb_context* context);
void next_state(state_t* state);
void init_state(state_t* state);
void finalize(state_t state, libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, int mcu_interface, int fpga_interface);
int commandloop(void);

int main(void) {

	/* Handle ctrl-c gracefully */
	signal(SIGINT, inthand);

	printStartupMsg();

	libusb_context* context = NULL;
	int rc = 0;

	/* Initialize libusb */
	debugprint("Initializing libusb...", DEFAULT);
	rc = libusb_init(&context);
	if (rc) {
		colorprint("ERROR: Failed to initialize libusb! Exiting...", RED);
		return EXIT_FAILURE;
	}

	libusb_set_debug(NULL, 3); //Set max debug level

	mainloop(context);

	libusb_exit(context);
}

void mainloop(libusb_context* context) {
	/* Program state */
	state_t state;
	
	init_state(&state);
	
	/* Return codes */
	//int rc = 0;

	/* Device handles */
	libusb_device_handle *mcu_handle, *fpga_handle; mcu_handle = fpga_handle = NULL;
	int mcu_interface, fpga_interface; mcu_interface = fpga_interface = 0;

	while (state.main_state != FINALIZE) {

		next_state(&state);

		if (state.main_state == GET_CMD) {
			state.cmd = commandloop();
		}

		if (_kill) {
			state.main_state = FINALIZE;
		}
	}

	finalize(state, mcu_handle, fpga_handle, mcu_interface, fpga_interface);
}

/* The world's largest and messiest switch...? */
/* TODO: Complete the switch cases*/

void next_state(state_t* state) {
	state_t next;
	/* Copy values, cmd not needed as it will not change */
	next.main_state = state->main_state;
	next.usb_state = state->usb_state;
	next.test = state->test;

	switch (state->main_state) {
		case INIT :
			next.main_state = GET_CMD;
			break;
		case GET_CMD :
			switch (state->usb_state) {
				case DISCONNECTED :
					switch (state->cmd) {
						case CONNECT :
							next.main_state = CONNECTING;
							next.usb_state = CONNECTING_ALL;
							break;
						case CONNECT_MCU :
							next.main_state = CONNECTING;
							next.usb_state = CONNECTING_MCU;
							break;
						case CONNECT_FPGA :
							next.main_state = CONNECTING;
							next.usb_state = CONNECTING_FPGA;
							break;
						case QUIT :
							next.main_state = FINALIZE;
							break;
						case HELP :
							next.main_state = GET_CMD;
							print_help_string();
							break;
						default :
							printf("No connected devices.\n");
							next.main_state = GET_CMD;
							break;
					}
					break;
				case CONNECTED :
					switch (state->cmd) {
						/* TODO: Add all appropriate cases */
						case RUN :
						case RUN_FPGA :
						case RUN_MCU :
						case STOP_FPGA :
						case STOP_MCU :
						case QUIT :
							next.main_state = FINALIZE;
							break;
						case HELP :
							print_help_string();
						default :
							/* Both devices already connected */
							next.main_state = GET_CMD;
							break;
					}
					break;
				case CONNECTED_MCU :
					switch (state->cmd) {
						/* TODO: Add all appropriate cases */
						case TESTSEND_N:
						case MCU_TESTSEND :
						case MCU_TESTSEND10 :
						case MCU_TESTRECV :
						case MCU_TESTRECV10 :
						case MCU_TESTSENDRECV :
						case QUIT :
						case HELP :
						default :
							printf("FPGA not connected. Try 'connect fpga'.");
							next.main_state = GET_CMD;
							break;
					}
					break;
				case CONNECTED_FPGA :
					switch (state->cmd) {
						case FPGA_TESTSEND :
						case FPGA_TESTSEND10 :
						case FPGA_TESTRECV :
						case FPGA_TESTRECV10 :
						case FPGA_TESTSENDRECV :
						case QUIT :
						case HELP :
						default :
							printf("MCU not connected. Try 'connect mcu'.");
							next.main_state = GET_CMD;
							break;
					}
					break;
				default :
					next.main_state = GET_CMD;
					break;
			}
			break;
		case CONNECTING :
			switch (state->usb_state) {
				case CONNECTED :
				case CONNECTED_MCU :
				case CONNECTED_FPGA :
					next.main_state = GET_CMD;
					break;
				default :
					/* TODO: Figure out reasonable default */
					next.main_state = GET_CMD;
					break;
			}
			break;
		case RUNNING :
			switch (state->cmd) {
				case STOP :
					next.main_state = STOPPING;
					break;
				default :
					next.main_state = GET_CMD;
			}
			break;
		case STOPPING :
			/* Stop all USB transactions */
		case TESTING :
			/* Running some test */
		default :
			/* TODO: Figure out a reasonable default case */
			next.main_state = GET_CMD;
	}
	
	/* Copy values back */
	state->main_state = next.main_state;
	state->usb_state = next.usb_state;
	state->test = next.test;

}

void init_state(state_t* state) {
	state->main_state = INIT;
	state->usb_state = DISCONNECTED;
	state->cmd = INVALID_CMD;
	state->test = NO_TEST;
}

void finalize(state_t state, libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, int mcu_interface, int fpga_interface) {

	if (state.usb_state == CONNECTED) {
		libusb_release_interface(mcu_handle, mcu_interface);
		libusb_release_interface(fpga_handle, fpga_interface);
		libusb_close(mcu_handle);
		libusb_close(fpga_handle);
	} else if (state.usb_state == CONNECTED_MCU) {
		libusb_release_interface(mcu_handle, mcu_interface);
		libusb_close(mcu_handle);
	} else if (state.usb_state == CONNECTED_FPGA) {
		libusb_release_interface(fpga_handle, fpga_interface);
		libusb_close(fpga_handle);
	}

	/* TODO: Close any remaining open files and free remaining allocated memory */
}

/* 
	Function to fetch commands from stdin in order to make the
	program interactive. Just add commands as they are implemented.
*/

int commandloop() {
	cmd_t cmd = INVALID_CMD;
	char stringBuffer[128]; //Unsafe, but who cares
	memset(stringBuffer, 0, 64);


	while (cmd == INVALID_CMD) {
		printf(">>> ");
		fgets(stringBuffer, 128, stdin);

		if (_kill) {
			cmd = QUIT;
			break;
		}

		for (int i = 0; stringBuffer[i]; i++) {
			stringBuffer[i] = tolower(stringBuffer[i]);
		}

		stringBuffer[strcspn(stringBuffer, "\r\n")] = 0; //remove trailing newline

		cmd = parse_cmd(stringBuffer);

		if (cmd == INVALID_CMD) {
			printf("Invalid command, try 'help'.\n");
		}
	}

	return cmd;
}
