#include "defs.h"
#include "debug.h"
#include "pthread_helper.h"
#include "test_usb_functs.h"
#include "usb_helpers.h"

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define OUTPUT_FP "resources/mcu_output.txt"

extern int _keepalive;
extern barrier_t barrier;

static unsigned char sendBuffer[512];
static unsigned char receiveBuffer[512];

static unsigned char tickMessage[] = "tick";
static int tickMessageLength = 4;

static unsigned char countMessage[] = "count";
static int countMessageLength = 5;

extern bool pendingWrite;
extern bool pendingReceive;

extern int _test;

void * mcu_runloop(void* pdata_void_ptr) {

	UNUSED(sendBuffer);
	UNUSED(receiveBuffer);
	UNUSED(tickMessage);
	UNUSED(tickMessageLength);
	UNUSED(countMessage);
	UNUSED(countMessageLength);

	/* Cast void ptr back to original type */
	pdata_t* pdata = (pdata_t*) pdata_void_ptr;

	FILE* f;
	f = fopen(OUTPUT_FP, "w");

	if (!f) {
		colorprint("ERROR: MCU_comm: Failed to open output file!", RED);
		return NULL;
	}

	/* Sync up with FPGA thread that is doing more heavy file I/O */
	barrier_wait(&barrier);

	int counter = 0;

	memset(receiveBuffer, 0, 512);

	while(_keepalive) {

		if (pendingWrite) {
			debugprint("Message waiting in send queue!", CYAN);
		} else {
			debugprint("Sending tick message.", GREEN);
			sendAsyncMessage(pdata->dev_handle, tickMessage, tickMessageLength);
		}

		if (pendingReceive) {
			debugprint("Still waiting for message!", MAGENTA);
		} else {
			debugprint("CONTENTS OF RECEIVEBUFFER:", BLUE);
			printf("receiveBuffer = %s\n", receiveBuffer);
			printf("counter = %d\n", counter);
			debugprint("==================", BLUE);
			debugprint("Setting up receive.", GREEN);
			memset(receiveBuffer, 0, 512);
			receiveAsyncMessage(pdata->dev_handle, receiveBuffer);
		}

		libusb_handle_events(pdata->context);
		usleep(30000);
	}

	debugprint("MCU_comm: Finished busy-wait. Closing open file.", DEFAULT);

	fclose(f);

	return NULL;
}

