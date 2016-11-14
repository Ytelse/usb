#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include "libusb.h"

#include "usb_device.h"
#include "callbacks.h"

#define UNUSED(x) (void) x

#define DBG_OUTPUT_FP "output/data_received.txt"

#define RECVTIMEOUT 1000

#define BUFFERSIZE 4096*4

#define TEST_DURATION 1.0f

unsigned char usb_recv_buffer[BUFFERSIZE];

bool pending_recv, pending_write;

/* Function prototypes */
static void recv_async(libusb_device_handle* dev_handle, unsigned char* buffer, int buflen);


static void recvdbgoutput(FILE* fout, double s, int recvcount, int loopcount, int sync_xferred);

int main(void) {
	/* General debug/test variables */
	struct timeval start, end;
	long int ms;
	double s;
	int rc, recvcount, sendcount, loopcount, sync_xferred;
	
	/* Debug output file */
	FILE* f;
	f = fopen(DBG_OUTPUT_FP, "w");
	assert(f);

	/* libusb variables */
	libusb_context* context = NULL;
	libusb_init(&context);
	libusb_set_debug(context, 3);

	/* Unused variables */
	UNUSED(sendcount);
	UNUSED(pending_write);

	libusb_device_handle* dev_handle = NULL;

	rc = connect(context, &dev_handle, 0);

	if (rc) {
		printf("Connection failed. Exiting...\n");
		return -1;
	}

	/* Do async receive test */
	recvcount = loopcount = sync_xferred = 0;
	memset(usb_recv_buffer, 0, BUFFERSIZE);
	gettimeofday(&start, NULL);
	while (true) {

		loopcount++;

		if (!pending_recv) {
			recv_async(dev_handle, usb_recv_buffer, BUFFERSIZE);
			recvcount++;
		}

		libusb_handle_events(context);

		gettimeofday(&end, NULL);
		ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		s = ms/1e6;
		if (s >= TEST_DURATION) {
			break;
		}
	}

	recvdbgoutput(stdout, s, recvcount, loopcount, sync_xferred);

	/* Do sync receive test */
	recvcount = loopcount = sync_xferred = 0;
	memset(usb_recv_buffer, 0, BUFFERSIZE);
	gettimeofday(&start, NULL);

	while (true) {
		loopcount++;
		int xferred;
		libusb_bulk_transfer(dev_handle, EP_IN, usb_recv_buffer, BUFFERSIZE, &xferred, RECVTIMEOUT);
		recvcount++;
		sync_xferred += xferred;

		gettimeofday(&end, NULL);
		ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		s = ms/1e6;
		if (s >= TEST_DURATION) {
			break;
		}
	}

	recvdbgoutput(stdout, s, recvcount, loopcount, sync_xferred);



	libusb_exit(context);
}

static void recv_async(libusb_device_handle* dev_handle, unsigned char* buffer, int buflen) {
	struct libusb_transfer* transfer = NULL;
	transfer = libusb_alloc_transfer(0);

	libusb_fill_bulk_transfer(
		transfer,
		dev_handle,
		EP_IN,
		buffer,
		buflen,
		&data_received_callback,
		NULL,
		RECVTIMEOUT
	);

	libusb_submit_transfer(transfer);

	pending_recv = true;
}

static void recvdbgoutput(FILE* fout, double s, int recvcount, int loopcount, int sync_xferred) {
	fprintf(fout, "========================================================\n");
	fprintf(fout, "Duration:                   %f\n", s);
	fprintf(fout, "Loops:                      %d\n", loopcount);
	fprintf(fout, "Messages received:          %d\n", recvcount);
	fprintf(fout, "Bytes/msg:                  %d\n", BUFFERSIZE);
	fprintf(fout, "Messages/second:            %f\n", (double)recvcount/s);
	fprintf(fout, "Data transferred:           %f Mb\n", (double)recvcount*BUFFERSIZE*8/1e6);
	fprintf(fout, "sync_xferred:               %f Mb\n", (double)sync_xferred*8/1e6);
	fprintf(fout, "Transfer speed:             %fMb/s\n", (double)(recvcount*BUFFERSIZE*8)/(s*1e6));
	fprintf(fout, "========================================================\n\n");

}







