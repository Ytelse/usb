#include "defs.h"
#include "debug.h"
#include "pacman_comm_setup.h"
#include "mcu_comm.h"
#include "fpga_comm.h"
#include "cmd_parser.h"
#include "pthread_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>

/* TODO: Remove following line */
int _test;

/* Global kill signal */
volatile sig_atomic_t _kill;
/* Thread keep-alive signal */
volatile int _keepalive;
/* Thread barrier for synchronizing */
barrier_t barrier;

void inthand(int signum) {
    _kill = 1;
}

/* Function prototypes */
void mainloop(libusb_context* context);
void next_state(state_t* state);
void init_state(state_t* state);
void finalize(state_t state, libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, int mcu_interface, int fpga_interface);
int run(state_t state, libusb_context* context, libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, int mcu_interface, int fpga_interface);
pacman_command_t commandloop(void);
void* control_thread(void* void_ptr);

int main(void) {
	/* Handle ctrl-c gracefully */
	signal(SIGINT, inthand);

	print_startup_msg();
	
	/* TODO: Remove following line */
	_test = 0;

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
	int rc = 0;

	/* Device handles */
	libusb_device_handle *mcu_handle, *fpga_handle; mcu_handle = fpga_handle = NULL;
	int mcu_interface, fpga_interface; mcu_interface = fpga_interface = 0;

	while (state.main_state != FINALIZE) {

		next_state(&state);

		switch (state.main_state) {
			case GET_CMD:
				state.cmd = commandloop();
				break;
			case CONNECTING :
				switch(state.usb_state) {
					case CONNECTING_ALL :
						/* TODO: Connect to both devices */
						/* For now just fall through to mcu connect */
					case CONNECTING_MCU :
						if (connect(context, &mcu_handle, PACMAN_MCU_DEVICE, &mcu_interface)) {
							state.usb_state = DISCONNECTED;
						} else {
							state.usb_state = CONNECTED_MCU;
						} 
						break;
					case CONNECTING_FPGA :
						if (connect(context, &fpga_handle, PACMAN_FPGA_DEVICE, &fpga_interface)) {
							state.usb_state = DISCONNECTED;
						} else {
							state.usb_state = CONNECTED_FPGA;
						}
						break;
					default :
						state.usb_state = DISCONNECTED;
				}
				break;
			case RUNNING :
				/* TODO: Add functionality for starting one at a time */
				switch (state.cmd.target) {
					case PACMAN_MCU_DEVICE :
					case PACMAN_FPGA_DEVICE :
					case PACMAN_BOTH_DEVICES :
					default :
						run(state, context, mcu_handle, fpga_handle, mcu_interface, fpga_interface);
				}
				break;
			case TESTING :
				/* Test connection to specified device by sending one transfer and expect one back */
				switch (state.cmd.target) {
					case PACMAN_MCU_DEVICE :
						rc = test_connection(mcu_handle, NULL, PACMAN_MCU_DEVICE);
					case PACMAN_FPGA_DEVICE : 
						rc = test_connection(NULL, fpga_handle, PACMAN_FPGA_DEVICE);
					case PACMAN_BOTH_DEVICES :
						rc = test_connection(mcu_handle, fpga_handle, PACMAN_BOTH_DEVICES);
					default :
						rc = test_connection(mcu_handle, NULL, PACMAN_MCU_DEVICE);
				}
				if (rc) {
					colorprint("ERROR: Connection test failed!", RED);
				} else {
					colorprint("Connection OK!", GREEN);
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
								case PACMAN_BOTH_DEVICES :
									next.usb_state = CONNECTING_ALL;
									break;
								case PACMAN_FPGA_DEVICE :
									next.usb_state = CONNECTING_FPGA;
									break;
								case PACMAN_MCU_DEVICE :
									next.usb_state = CONNECTING_MCU;
									break;
								default :
									next.usb_state = DISCONNECTED;

							}
							break;
						case QUIT :
							next.main_state = FINALIZE;
							break;
						case ART:
							next.main_state = GET_CMD;
							print_startup_msg();
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
							next.main_state = RUNNING;
							break;
						case TEST_CONNECTION:
							next.main_state = TESTING;
							break;
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
						case RUN : /* TODO: Remove this, only for testing */ 
							next.main_state = RUNNING;
							break;
						case TEST_CONNECTION:
							next.main_state = TESTING;
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
						case QUIT :
							next.main_state = FINALIZE;
							break;
						case TEST_CONNECTION :
							next.main_state = TESTING;
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

}

void init_state(state_t* state) {
	state->main_state = INIT;
	state->usb_state = DISCONNECTED;
	state->cmd.command = INVALID_CMD;
	state->cmd.target = PACMAN_NO_DEVICE;
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

int run(state_t state, libusb_context* context, libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, int mcu_interface, int fpga_interface) {
	int rc;
	pthread_t fpga_thread, mcu_thread, ctrl_thread;
	_keepalive = 1;

	pdata_t fpga_data, mcu_data;

	/* Initialize structs */
	fpga_data.state = &state;
	fpga_data.context = context;
	fpga_data.dev_handle = fpga_handle;
	fpga_data.dev_interface = &fpga_interface;

	mcu_data.state = &state;
	mcu_data.context = context;
	mcu_data.dev_handle = mcu_handle;
	mcu_data.dev_interface = &mcu_interface;

	/* Initialize barrier(s) */
	barrier_init(&barrier, 3);

	debugprint("Creating threads...", DEFAULT);

	/* Spawn FPGA thread */
	rc = pthread_create(&fpga_thread, NULL, fpga_runloop, (void*)&fpga_data);
	if (rc) {
		colorprint("ERROR: pthread_create() failed!", RED);
		return rc;
	}
	/* Spawn MCU thread */
	rc = pthread_create(&mcu_thread, NULL, mcu_runloop, (void*)&mcu_data);
	if (rc) {
		colorprint("ERROR: pthread_create() failed!", RED);
		return rc;
	}
	/* Spawn control thread */
	rc = pthread_create(&ctrl_thread, NULL, control_thread, NULL);
	if (rc) {
		colorprint("ERROR: pthread_create() failed!", RED);
		return rc;
	}

	pthread_join(fpga_thread, NULL);
	pthread_join(mcu_thread, NULL);
	pthread_join(ctrl_thread, NULL);

	debugprint("Threads joined.", GREEN);
	return 0;
}

/* A thread used only for being able to stop the other two threads. Not very pretty. */
void* control_thread(void* void_ptr) {
	pacman_command_t cmd;

	/* Wait for the other 2 threads */
	barrier_wait(&barrier);

	while (_keepalive) {
		cmd = commandloop();
		if (cmd.command == STOP) {
			_keepalive = 0;
		} else {
			printf("Only available command in run mode is 'stop'.\n");
		}
	}

	return NULL;
}

/* 
	Function to fetch commands from stdin in order to make the
	program interactive. Just add commands as they are implemented.
*/

pacman_command_t commandloop() {
	pacman_command_t cmd;
	cmd.command = INVALID_CMD;
	cmd.target = PACMAN_NO_DEVICE;

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
