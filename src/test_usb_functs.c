#include "debug.h"
#include "usb_helpers.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// /* Data to send to device every tick */
static unsigned char sendBuffer[512];
static unsigned char receiveBuffer[512];

static unsigned char tickMessage[] = "tick";
static int tickMessageLength = 4;

extern bool pendingWrite;
extern bool pendingReceive;

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

void sendNTicks(libusb_context* context, libusb_device_handle* efm_handle, int num_ticks) {
	for (int i = 0; i < num_ticks; i++) {
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