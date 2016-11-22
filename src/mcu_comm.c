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

static unsigned char tickMessage[] = "tick";
static int tickMessageLength = 4;

static unsigned char countMessage[] = "count";
static int countMessageLength = 5;

extern bool pendingWrite;
extern bool pendingReceive;


void * mcu_runloop(void* pdata_void_ptr) {

	struct timeval start, end;

	UNUSED(sendBuffer);
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

	/* Send start message */
	unsigned char start_msg[] = "start";
	send_async_transfer(pdata->dev_handle, start_msg, 5*sizeof(unsigned char), 10000);
	/* Wait until the message is received on the other end */
	while(pendingWrite) {
	  libusb_handle_events(pdata->context);
	}


	int sendcounter = 0;
	int recvcounter = 0;
	int mcurecvcounter = 0;
	memset(receiveBuffer, 0, 4096*4);

	long int ms;
	double s;


	gettimeofday(&start, NULL);

	while(_keepalive) {
		if (!pendingReceive) {
			recv_async_transfer(pdata->dev_handle, receiveBuffer, 4096*4, 10000);
		}
		
		libusb_handle_events(pdata->context);
	}
		

	/* _keepalive should be 0 at this point, so send the stop signal */
	unsigned char stop_msg[] = "stop";
	send_async_transfer(pdata->dev_handle, stop_msg, 4*sizeof(unsigned char), 1000);

	gettimeofday(&end, NULL);
	ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    s = ms/1e6;

    float msg_sent_per_sec = ((float) sendcounter / s);
    float msg_recv_per_sec = ((float) recvcounter / s);
    float total_per_sec = ((float) (sendcounter + recvcounter)) / s;

    printf("========================================\n");
    printf("Duration:                  %f\n", s);
    printf("Messages sent:             %d\n", sendcounter);
    printf("Messages received:         %d\n", recvcounter);
    //printf("Last message received:     %s\n", mcuMsgBuf);
    //printf("MCU receive counter:       %d\n", mcurecvcounter);
    //printf("Messages sent/second:      %f\n", msg_sent_per_sec);
    printf("Messages received/second:  %f\n", msg_recv_per_sec);
    //printf("Total/second:              %f\n", total_per_sec);
    printf("========================================\n");

    while(_keepalive) {
    	/* Do nothing */
    }

	debugprint("MCU_comm: Finished busy-wait. Closing open file.", DEFAULT);


	return NULL;
}

