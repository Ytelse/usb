#include "defs.h"
#include "debug.h"
#include "pthread_helper.h"
#include "usb_helpers.h"

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

	int sendcounter = 0;
	int recvcounter = 0;
	int mcurecvcounter = 0;
	memset(receiveBuffer, 0, 4096*8);

	long int ms;
	double s;

	gettimeofday(&start, NULL);

	while(_keepalive) {

		// if (pendingWrite) {
		// 	debugprint("Message waiting in send queue!", CYAN);
		// } else {
		// 	debugprint("Sending tick message.", GREEN);
		// 	sendAsyncMessage(pdata->dev_handle, tickMessage, tickMessageLength);
		// 	sendcounter++;
		// }

		// if (pendingReceive) {
		// 	debugprint("Still waiting for message!", MAGENTA);
		// } else {
		// 	debugprint("CONTENTS OF RECEIVEBUFFER:", BLUE);
		// 	printf("receiveBuffer = %s\n", receiveBuffer);
		// 	printf("counter = %d\n", counter);
		// 	debugprint("==================", BLUE);
		// 	debugprint("Setting up receive.", GREEN);
		// 	memset(receiveBuffer, 0, 512);
		// 	receiveAsyncMessage(pdata->dev_handle, receiveBuffer);
		// 	recvcounter++;
		// }

		// if (!pendingWrite) {
		// 	sendAsyncMessage(pdata->dev_handle, tickMessage, tickMessageLength);
		// 	sendcounter++;
		// }

		if (!pendingReceive) {
			fprintf(f, "%s\n", receiveBuffer);
			//memset(receiveBuffer, 0, 512);
			recv_async_transfer(pdata->dev_handle, receiveBuffer, 4096, 1000);
			recvcounter++;
		}
		libusb_handle_events(pdata->context);
		//usleep(30000);
		gettimeofday(&end, NULL);
		ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		s = ms/1e6;

		if (s >= 1.0f) {
			break;
		} 
	}

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

	fclose(f);

	return NULL;
}

