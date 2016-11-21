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
#include <i_defs.h>

/* Global kill signal */
volatile sig_atomic_t _kill;
/* Thread keep-alive signal */
volatile int _keepalive;
/* Thread barrier for synchronizing */
barrier_t barrier;

int** result_buffer;

void inthand(int signum) {
    _kill = 1;
}

/* Function prototypes */
void mainloop(libusb_context* context);
void init_state(state_t* state);
void finalize(state_t state, libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, int mcu_interface, int fpga_interface);
int run(state_t state, libusb_context* context, libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, int mcu_interface, int fpga_interface);
pacman_command_t commandloop(void);
void* control_thread(void* void_ptr);

int main(void) {
	/* Handle ctrl-c gracefully */
	signal(SIGINT, inthand);

	print_startup_msg();

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

	result_buffer = (int**)malloc(NOF_IMAGES * sizeof(int));
	for (int i = 0; i < NOF_IMAGES; i++) {
	  result_buffer[i] = (int*)malloc(4096 * sizeof(int));
	}


	mainloop(context);

	libusb_exit(context);
}

void mainloop(libusb_context* context) {
	/* Program state */
	state_t state;

	init_state(&state);

	/* Device handles */
	libusb_device_handle *mcu_handle, *fpga_handle; mcu_handle = fpga_handle = NULL;
	int mcu_interface, fpga_interface; mcu_interface = fpga_interface = 0;

	if (connect(context, &mcu_handle, PACMAN_MCU_DEVICE, &mcu_interface)) {
	  puts("EXIT PROGRAM: Error connecting to MCU");
	  exit(1);
	}

	state.usb_state = CONNECTED_MCU;

	if (connect(context, &fpga_handle, PACMAN_FPGA_DEVICE, &fpga_interface)) {
	  puts("EXIT PROGRAM: Error connecting to FPGA");
	  exit(1);
	}

	state.usb_state = CONNECTED;

	run(state, context, mcu_handle, fpga_handle, mcu_interface, fpga_interface);

	state.main_state = RUNNING;

	finalize(state, mcu_handle, fpga_handle, mcu_interface, fpga_interface);

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
