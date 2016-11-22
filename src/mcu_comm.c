#include "defs.h"
#include "debug.h"
#include "pthread_helper.h"
#include "usb_helpers.h"
#include "callbacks.h"
#include "i_defs.h"

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#define OUTPUT_FP "resources/mcu_output.txt"

extern int _keepalive;
extern barrier_t barrier;

static unsigned char sendBuffer[512];
static unsigned char receiveBuffer[4096*8];

extern bool pendingWrite;
extern bool pendingReceive;


void * mcu_runloop(void* pdata_void_ptr) {
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

	/* Send start message */
	unsigned char start_msg[] = "start";
	send_async_transfer(pdata->dev_handle, start_msg, 5*sizeof(unsigned char), 10000);
	/* Wait until the message is received on the other end */
	while(pendingWrite) {
	  libusb_handle_events(pdata->context);
	}


	memset(receiveBuffer, 0, 4096*4);


	while(_keepalive) {
		if (!pendingReceive) {
			recv_async_transfer(pdata->dev_handle, receiveBuffer, 4096*4, 10000);
		}
		libusb_handle_events(pdata->context);
	}
		

	/* _keepalive should be 0 at this point, so send the stop signal */
	unsigned char stop_msg[] = "stop";
	send_async_transfer(pdata->dev_handle, stop_msg, 4*sizeof(unsigned char), 1000);


    while(_keepalive) {
    	/* Do nothing */
    }

	debugprint("MCU_comm: Finished busy-wait. Closing open file.", DEFAULT);


	return NULL;
}

