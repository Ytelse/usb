#include "debug.h"
#include "ytelse_comm.h"
#include "cmd_parser.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include <ctype.h>
#include <signal.h>

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
	/* MCU tests, add more as needed */
	SEND_TEST,
	RECV_TEST,
	SENDRECV_TEST,
	/* Initial value */
	NO_TEST
} test_t;

typedef struct State {
	main_state_t main_state; /* State of mainloop */
	usb_state_t usb_state;
	ytelse_command_t cmd;
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
ytelse_command_t commandloop(void);

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

		switch(state.main_state) {
			case GET_CMD:
				state.cmd = commandloop();
				break;
			case CONNECTING :
				switch(state.usb_state) {
					case CONNECTING_ALL :
						/* TODO: Connect to both devices */
					case CONNECTING_MCU :
						if (connect(context, &mcu_handle, YTELSE_MCU_DEVICE, &mcu_interface)) {
							state.usb_state = DISCONNECTED;
						} else {
							state.usb_state = CONNECTED_MCU;
						} 
						break;
					case CONNECTING_FPGA :
						if (connect(context, &fpga_handle, YTELSE_FPGA_DEVICE, &fpga_interface)) {
							state.usb_state = DISCONNECTED;
						} else {
							state.usb_state = CONNECTED_FPGA;
						}
						break;
					default :
						state.usb_state = DISCONNECTED;
				}
			case TESTING :
				switch (state.test) {
					case SEND_TEST : /* etc */
					default :
						state.test = NO_TEST;
						break;

				}
				break;
			default :
				/* Do nothing */
				state.cmd = commandloop();
				break;
		}

		if (_kill || state.cmd.command == QUIT) {
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
					switch (state->cmd.command) {
						case CONNECT :
							next.main_state = CONNECTING;
							switch (state->cmd.target) {
								case YTELSE_BOTH_DEVICES :
									next.usb_state = CONNECTING_ALL;
									break;
								case YTELSE_FPGA_DEVICE :
									next.usb_state = CONNECTING_FPGA;
									break;
								case YTELSE_MCU_DEVICE :
									next.usb_state = CONNECTING_MCU;
									break;
								default :
									next.usb_state = DISCONNECTED;

							}
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
					switch (state->cmd.command) {
						/* TODO: Add all appropriate cases */
						case RUN :
						case STOP :
						case CONNECT : /* In case connect to 1 device and want to connect other */
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
					switch (state->cmd.command) {
						/* TODO: Add all appropriate cases */
						case TESTSEND :
						case TESTRECV :
						case TESTSENDRECV :
						case QUIT :
							next.main_state = FINALIZE;
							break;
						case HELP :
							print_help_string();
						default :
							next.main_state = GET_CMD;
							break;
					}
					break;
				case CONNECTED_FPGA :
					switch (state->cmd.command) {
						case TESTSEND :
						case TESTRECV :
						case TESTSENDRECV :
						case QUIT :
							next.main_state = FINALIZE;
							break;
						case HELP :
							print_help_string();
						default :
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
			// switch (state->usb_state) {
			// 	case CONNECTED :
			// 	case CONNECTED_MCU :
			// 	case CONNECTED_FPGA :
			// 		next.main_state = GET_CMD;
			// 		break;
			// 	default :
			// 		/* TODO: Figure out reasonable default */
			// 		next.main_state = GET_CMD;
			// 		break;
			// }
			next.main_state = GET_CMD;
			break;
		case RUNNING :
			switch (state->cmd.command) {
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
			next.main_state = GET_CMD;
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
	state->cmd.command = INVALID_CMD;
	state->cmd.target = YTELSE_NO_DEVICE;
	state->cmd.N = -1;
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

ytelse_command_t commandloop() {
	ytelse_command_t cmd;
	cmd.command = INVALID_CMD;
	cmd.target = YTELSE_NO_DEVICE;
	cmd.N = -1;

	char stringBuffer[128]; //Unsafe, but who cares
	memset(stringBuffer, 0, 64);

	while (cmd.command == INVALID_CMD) {
		printf(">>> ");
		fgets(stringBuffer, 128, stdin);

		if (_kill) {
			cmd.command = QUIT;
			break;
		}

		for (int i = 0; stringBuffer[i]; i++) {
			stringBuffer[i] = tolower(stringBuffer[i]);
		}

		stringBuffer[strcspn(stringBuffer, "\r\n")] = 0; //remove trailing newline

		cmd = parse_cmd(stringBuffer);

		if (cmd.command == INVALID_CMD) {
			printf("Invalid command, try 'help'.\n");
		}
	}

	return cmd;
}
