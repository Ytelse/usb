#include "debug.h"
#include "ytelse_comm.h"
#include "usb_helpers.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <libusb.h>
#include <ctype.h>

// /* Data to send to device every tick */
static unsigned char sendBuffer[512];
static unsigned char receiveBuffer[512];

static unsigned char tickMessage[] = "tick";
static int tickMessageLength = 4;

extern bool pendingWrite;
extern bool pendingReceive;

/* enum for main loop flow control */
enum state {
	USB_FINALIZE,
	USB_DEVICE_CONNECTED,
	USB_DEVICE_DISCONNECTED,
	USB_DEVICE_FOUND,
	USB_DEVICE_NOT_FOUND,
	USB_DEVICE_INTERFACE_CLAIMED, 
	USB_DEVICE_INTERFACE_BUSY, 
	USB_DEVICE_INTERFACE_NOT_FOUND
};

/* enum for run-time commands, add more as necessary */

enum commands {
	INVALID_CMD, 	/* No command selected */
	TESTSEND,		/* Send 1 message to MCU */
	TESTRECV,		/* Set up receive of 1 message from MCU */
	TESTRECV10, 	/* Set up receive of 10 messages from MCU */
	TESTSENDRECV,	/* Send and set up receive of 1 message to/from MCU */
	HELP, 			/* Print available commands */
	QUIT			/* Quit the program */
};

/* Function prototypes */
void mainloop(libusb_context* context);
int commandloop(void);

/* Test functions */
void sendTick(libusb_context* context, libusb_device_handle* efm_handle);
void testRecv(libusb_context* context, libusb_device_handle* efm_handle);
void testSendRecv(libusb_context* context, libusb_device_handle* efm_handle, int num_messages);
void sendRecvWait(libusb_context* context, libusb_device_handle* efm_handle);
void receiveNMsgs(libusb_context* context, libusb_device_handle* efm_handle, int num_recvs);


/* Print helpers */
void printStartupMsg(void);
void printHelpString(void);

int main(void) {
	printStartupMsg();

	libusb_context* context = NULL;
	int rc = 0;

	/* Initialize libusb */
	colorprint("Initializing libusb...", DEFAULT);
	rc = libusb_init(&context);
	if (rc) {
		colorprint("Failed to initialize libusb! Exiting...", RED);
		return EXIT_FAILURE;
	}

	libusb_set_debug(NULL, 3); //Set max debug level

	mainloop(context);

	libusb_exit(context);
}

void mainloop(libusb_context* context) {

	int status = USB_DEVICE_NOT_FOUND;
	int cmd = INVALID_CMD;
	int interface = 0;
	int rc = 0;
	libusb_device_handle* efm_handle = NULL;

	while (status != USB_FINALIZE) {
		/* Get device handle */
		colorprint("Establishing connection to EFM32 USB device...", DEFAULT);
		// Keep trying until it connects
		while(get_ytelse_mcu_handle(context, &efm_handle));

		debugprint("Successfully got EFM32 handle!", GREEN);
		status = USB_DEVICE_FOUND;

		/* Claim device USB interface */
		debugprint("Claiming EFM32 USB interface...", DEFAULT);
		while(status != USB_DEVICE_INTERFACE_CLAIMED) {
			// Request resource from OS 
			rc = libusb_claim_interface(efm_handle, interface);
			
			if (rc == LIBUSB_ERROR_NOT_FOUND) {
				/* 
					Interface not found on current device. Should not happen.
					Just in order to do something, increment interface number
					and try again.
					TODO: Test this approach
				*/
				debugprint("Interface not found! Trying another...", RED);
				status = USB_DEVICE_INTERFACE_NOT_FOUND;
				interface++;
				interface = interface % 10;
			} else if (rc == LIBUSB_ERROR_BUSY) {
				/* 
					USB device claimed by some other process. Should not be a problem.
					Do nothing, wait for it to become free.
				*/
				debugprint("Device busy, trying again...", YELLOW);
				status = USB_DEVICE_INTERFACE_BUSY;
				sleep(1); //Wait a second before trying again.
			} else if (rc == LIBUSB_ERROR_NO_DEVICE) {
				/*
					Device has disconnected, and we need to find it again.
					Break from interface claiming loop and return to waiting
					for it to connect.
				*/
				debugprint("Device disconnected!", RED);
				status = USB_DEVICE_NOT_FOUND;
				break;
			} else {
				debugprint("Successfully claimed EFM32 USB interface!", GREEN);
				status = USB_DEVICE_INTERFACE_CLAIMED;
			}
		}

		char nameBuffer[200];
		int nameLength = getDeviceName(efm_handle, nameBuffer, 200);
		if (nameLength < 0) {
			colorprint("Failed to get device name...", YELLOW);
		} else {
			colorprint("Connected to:", BLUE);
			colorprint(nameBuffer, BLUE);
		}
		colorprint("Connection established!", GREEN);

		while (cmd != QUIT) {
			cmd = commandloop();
			switch (cmd) {
				case TESTSEND :
					sendTick(context, efm_handle);
					break;
				case TESTRECV :
					testRecv(context, efm_handle);
					break;
				case TESTRECV10 :
					receiveNMsgs(context, efm_handle, 10);
					break;
				case TESTSENDRECV :
					sendRecvWait(context, efm_handle);
					break;
				case HELP :
					printHelpString();
					break;
				case QUIT :
					colorprint("Exiting...", YELLOW);
					break;
				default :
					/* If unrecognized cmd, just quit. Should not happed */
					cmd = QUIT;
					colorprint("Exiting...", YELLOW);
					break;
			}
		}
		status = USB_FINALIZE;
	}
	//Release previously claimed interface
	libusb_release_interface(efm_handle, interface);
	//Close connection to device
	libusb_close(efm_handle);
}

/* 
	Function to fetch commands from stdin in order to make the
	program interactive. Just add commands as they are implemented.
*/

int commandloop() {
	int cmd = INVALID_CMD;
	char stringBuffer[64];
	memset(stringBuffer, 0, 64);


	while (cmd == INVALID_CMD) {
		printf(">>> ");
		fgets(stringBuffer, 64, stdin);

		for (int i = 0; stringBuffer[i]; i++) {
			stringBuffer[i] = tolower(stringBuffer[i]);
		}

		stringBuffer[strcspn(stringBuffer, "\r\n")] = 0; //remove trailing newline

		if (strcmp(stringBuffer, "testsend") == 0) {
			cmd = TESTSEND;
		} else if (strcmp(stringBuffer, "ts") == 0) {
			cmd = TESTSEND;
		} else if (strcmp(stringBuffer, "testrecv") == 0) {
			cmd = TESTRECV;
		} else if (strcmp(stringBuffer, "tr") == 0) {
			cmd = TESTRECV;
		} else if (strcmp(stringBuffer, "testrecv10") == 0) {
			cmd = TESTRECV10;
		} else if (strcmp(stringBuffer, "tr10") == 0) {
			cmd = TESTRECV10;
		} else if (strcmp(stringBuffer, "testsendrecv") == 0) {
			cmd = TESTSENDRECV;
		} else if (strcmp(stringBuffer, "tsr") == 0) {
			cmd = TESTSENDRECV;
		} else if (strcmp(stringBuffer, "help") == 0) {
			cmd = HELP;
		} else if (strcmp(stringBuffer, "quit") == 0) {
			cmd = QUIT;
		} else  if (strcmp(stringBuffer, "exit") == 0) {
			cmd = QUIT;
		} else {
			printf("Invalid command, try 'help'.\n");
		}
	}

	return cmd;
}

/* Test function, sends 50 messages to USB device, and sets up continuous receives */
void testSendRecv(libusb_context* context, libusb_device_handle* efm_handle, int num_messages) {
	memset(receiveBuffer, 0, 512);

	for (int i = 0; i < num_messages; i++) {
		debugprint("Attempting to send tick message to EFM32", BLUE);
		if (pendingWrite) {
			debugprint("Message already waiting in queue!", RED);
		} else {
			sendAsyncMessage(efm_handle, tickMessage, tickMessageLength);
			debugprint("Message sent!", GREEN);
		}

		if (pendingReceive) {
			debugprint("Still waiting for message!", RED);
		} else {
			debugprint("Setting up receive...", GREEN);
			memset(receiveBuffer, 0, 512);
			receiveAsyncMessage(efm_handle, receiveBuffer);
		}

		printf("receiveBuffer = %s\n", receiveBuffer);

		libusb_handle_events(context);
		usleep(500000);
	}
}

void sendTick(libusb_context* context, libusb_device_handle* efm_handle) {
	while (1) {
		if (!pendingWrite) {
			sendAsyncMessage(efm_handle, tickMessage, tickMessageLength);
			break;
		}
	}

	while (pendingWrite) {
		libusb_handle_events(context);
	}
}

void testRecv(libusb_context* context, libusb_device_handle* efm_handle) {
	while(1) {
		if (!pendingReceive) {
			memset(receiveBuffer, 0, 512);
			receiveAsyncMessage(efm_handle, receiveBuffer);
			break;
		}
	}

	while(pendingReceive) {
		libusb_handle_events(context);
	}

	printf("Received message: %s\n", receiveBuffer);
}

/* Test function : send 1 message, recv 1 message */

void sendRecvWait(libusb_context* context, libusb_device_handle* efm_handle) {
	debugprint("Attempting to send tick message to Ytelse MCU", BLUE);
	while (1) {
		if (!pendingWrite) {
			sendAsyncMessage(efm_handle, tickMessage, tickMessageLength);
			break;
		}
	}
	debugprint("Message sent!", GREEN);
	while (1) {
		if (!pendingReceive) {
			memset(receiveBuffer, 0, 512);
			receiveAsyncMessage(efm_handle, receiveBuffer);
			break;
		}
	}

	while (pendingReceive) {
		libusb_handle_events(context);
	}

	printf("Received message: %s\n", receiveBuffer);
}

void receiveNMsgs(libusb_context* context, libusb_device_handle* efm_handle, int num_recvs) {

	for (int i = 0; i < num_recvs; i++) {
		while (1) {
			if (!pendingReceive) {
				memset(receiveBuffer, 0, 512);
				receiveAsyncMessage(efm_handle, receiveBuffer);
				break;
			}
		}
		while (pendingReceive) {
			libusb_handle_events(context);
		}
		printf("Received message: %s\n", receiveBuffer);
	}
}

/* Print convenience functions */

void printStartupMsg(void) {

	/* TODO: Horrible ASCII art, make it better */
	printf("\n");
	printf("\n");
	printf("********************************************\n");
	printf("*           USB Host for Ytelse            *\n");
	printf("********************************************\n");
	printf("* /^^^^^|     /^^^^^^|       /^^^^^^^^^^|  *\n");
	printf("* | MCU | === | FPGA | ====  | USB Host |  *\n");
	printf("* |_____/     |______/       |__________   *\n");
	printf("*   ||                           ||        *\n");
	printf("*   ||                           ||        *\n");
	printf("*    ============================//        *\n");
	printf("********************************************\n");
	#ifdef DEBUG
	printf("*               DEBUG MODE                 *\n");
	printf("********************************************\n");
	#endif
}

void printHelpString(void) {
	printf("\n");
	colorprint("Available commands: ", MAGENTA);
	printf("testsend, ts        --  Send 1 message to MCU\n");
	printf("testrecv, tr        --  Set up receive of 1 message from MCU\n");
	printf("testrecv10, tr10    --  Set up receive of 10 messages from MCU\n");
	printf("testsendrecv, tsr   --  Send and set up receive of 1 message to/from MCU\n");
	printf("quit, exit          --  Quit the program\n");
	printf("help                --  Print list of available commands\n");
	printf("\n");
}