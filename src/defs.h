#ifndef __DEFS_H_
#define __DEFS_H_

#include <libusb.h>

typedef unsigned char byte_t;

/* Device types */
typedef enum PacmanDevices {
	PACMAN_MCU_DEVICE,
	PACMAN_FPGA_DEVICE,
	PACMAN_BOTH_DEVICES,
	PACMAN_NO_DEVICE}
pacman_device_t;

typedef enum commands {
	/* Useful commands */
	INVALID_CMD, 		/* No command selected */
	CONNECT,			/* Connect to specified device(s) */
	RUN,				/* Run  */
	STOP,				/* Stop whatever transactions are going on between host and given device (and return to command fetch loop) */
	HELP, 				/* Print available commands */
	ART,				/* Print startup image */
	QUIT,				/* Quit the program */
	TEST_CONNECTION		/* Test the connection to one or both devices */
} cmd_t;

typedef struct Command {
	cmd_t command;
	pacman_device_t target;
} pacman_command_t;

/* enum for main loop flow control */
typedef enum MainloopState {
	/* Overall */
	INIT,							/* Initial state */
	FINALIZE, 						/* Exit state, stop all communication */
	CONNECTING,						/* Connect to one or more devices */
	RUNNING,						/* Host is sending data to FPGA and receiving results from MCU */
	STOPPING,						/* Halt the program without releasing devices and quitting */
	GET_CMD,						/* Waiting for command input */
	TESTING,						/* Testing connection to device(s) */
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

typedef struct State {
	main_state_t main_state; /* State of mainloop */
	usb_state_t usb_state;
	pacman_command_t cmd;
} state_t;

typedef struct PthreadData {
	state_t* state;
	libusb_context* context;
	libusb_device_handle* dev_handle;
	int* dev_interface;
} pdata_t;

#endif /* __DEFS_H_ */

