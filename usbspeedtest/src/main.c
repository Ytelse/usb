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

#define DBG_OUTPUT_FP "dbg_out.txt"

#define RECVTIMEOUT 1000
#define SENDTIMEOUT 1000

#define BUFFERSIZE_R 4096*4
#define BUFFERSIZE_S 4096

#define TEST_DURATION 2.0f

enum {SEND, RECV, BOTH};

unsigned char usb_recv_buffer[BUFFERSIZE_R];
unsigned char usb_send_buffer[BUFFERSIZE_S];

bool pending_recv, pending_write;

/* Function prototypes */
static void recv_async(libusb_device_handle* dev_handle, unsigned char* buffer, int buflen);
static void send_async(libusb_device_handle* dev_handle, unsigned char* buffer, int buflen);

static void recvtest(libusb_context* context, libusb_device_handle* dev_handle, FILE* fout);
static void sendtest(libusb_context* context, libusb_device_handle* dev_handle, FILE* fout);

static void recvdbgoutput(FILE* fout, double s, int recvcount, int loopcount, int sync_xferred);
static void senddbgoutput(FILE* fout, double s, int sendcount, int loopcount, int sync_xferred);

int main(int argc, char** argv) {
	int rc, mode;

	if (argc < 2) {
		printf("Usage: usbspeed [send | recv | both]\n");
		printf("Defaulting to BOTH.\n\n");
		mode = BOTH;
	} else {
		if (strcmp(argv[2], "send") == 0) {
			mode = SEND;
		} else if (strcmp(argv[2], "recv") == 0) {
			mode = RECV;
		} else if (strcmp(argv[2], "both") == 0) {
			mode = BOTH;
		} else {
			printf("Usage: usbspeed [send | recv | both], default: 'both'.\n");
			return 0;
		}
	}

	/* Debug output file */
	FILE* f;
	f = fopen(DBG_OUTPUT_FP, "w");
	assert(f);

	/* libusb variables */
	libusb_context* context = NULL;
	libusb_init(&context);
	libusb_set_debug(context, 3);

	libusb_device_handle* dev_handle = NULL;

	rc = connect(context, &dev_handle, 0);

	if (rc) {
		printf("Connection failed. Exiting...\n");
		return -1;
	}

	if (mode == RECV || mode == BOTH) {
		recvtest(context, dev_handle, stdout);
	}

	if (mode == SEND || mode == BOTH) {
		sendtest(context, dev_handle, stdout);
	}

	libusb_release_interface(dev_handle, 0);
	libusb_close(dev_handle);
	libusb_exit(context);
}

static void recvtest(libusb_context* context, libusb_device_handle* dev_handle, FILE* fout) {
	fprintf(fout, "========================================================\n");
	fprintf(fout, "                        RECV TEST\n");
	fprintf(fout, "========================================================\n");

	struct timeval start, end;
	long int ms;
	double s;
	int recvcount, loopcount, sync_xferred;
	fprintf(fout, "                        ASYNC\n");
	/* Async receive test */
	recvcount = loopcount = sync_xferred = 0;
	memset(usb_recv_buffer, 0, BUFFERSIZE_R);
	gettimeofday(&start, NULL);
	while (true) {

		loopcount++;

		if (!pending_recv) {
			recv_async(dev_handle, usb_recv_buffer, BUFFERSIZE_R);
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

	recvdbgoutput(fout, s, recvcount, loopcount, sync_xferred);
	fprintf(fout, "                        SYNC\n");
	/* Sync receive test */
	recvcount = loopcount = sync_xferred = 0;
	memset(usb_recv_buffer, 0, BUFFERSIZE_R);
	gettimeofday(&start, NULL);

	while (true) {
		loopcount++;
		int xferred;
		libusb_bulk_transfer(dev_handle, EP_IN, usb_recv_buffer, BUFFERSIZE_R, &xferred, RECVTIMEOUT);
		recvcount++;
		sync_xferred += xferred;

		gettimeofday(&end, NULL);
		ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		s = ms/1e6;
		if (s >= TEST_DURATION) {
			break;
		}
	}

	recvdbgoutput(fout, s, recvcount, loopcount, sync_xferred);
}

static void sendtest(libusb_context* context, libusb_device_handle* dev_handle, FILE* fout) {

	fprintf(fout, "========================================================\n");
	fprintf(fout, "                        SEND TEST\n");
	fprintf(fout, "========================================================\n");

	struct timeval start, end;
	long int ms;
	double s;
	int sendcount, loopcount, sync_xferred;
	sendcount = loopcount = sync_xferred = 0;
	memset(usb_send_buffer, 0xF, BUFFERSIZE_S);
	gettimeofday(&start, NULL);

	/* Async send test */
	fprintf(fout, "                        ASYNC\n");
	while (true) {
		loopcount++;

		if (!pending_write) {
			send_async(dev_handle, usb_send_buffer, BUFFERSIZE_S);
			sendcount++;
		}

		libusb_handle_events(context);

		gettimeofday(&end, NULL);
		ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		s = ms/1e6;
		if (s >= TEST_DURATION) {
			break;
		}
	}

	senddbgoutput(fout, s, sendcount, loopcount, sync_xferred);

	/* Sync send test */
	fprintf(fout, "                        SYNC\n");
	sendcount = loopcount = sync_xferred = 0;
	gettimeofday(&start, NULL);

	while (true) {
		loopcount++;
		int xferred;
		libusb_bulk_transfer(dev_handle, EP_OUT, usb_send_buffer, BUFFERSIZE_S, &xferred, SENDTIMEOUT);
		sendcount++;
		sync_xferred += xferred;

		gettimeofday(&end, NULL);
		ms = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		s = ms/1e6;
		if (s >= TEST_DURATION) {
			break;
		}
	}

	senddbgoutput(fout, s, sendcount, loopcount, sync_xferred);
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

static void send_async(libusb_device_handle* dev_handle, unsigned char* buffer, int buflen) {
	struct libusb_transfer* transfer = NULL;
	transfer = libusb_alloc_transfer(0);

	libusb_fill_bulk_transfer(
		transfer,
		dev_handle,
		EP_OUT,
		buffer,
		buflen,
		&data_sent_callback,
		NULL,
		SENDTIMEOUT
	);

	libusb_submit_transfer(transfer);

	pending_write = true;
}

static void recvdbgoutput(FILE* fout, double s, int recvcount, int loopcount, int sync_xferred) {
	fprintf(fout, "========================================================\n");
	fprintf(fout, "Duration:                   %f\n", s);
	fprintf(fout, "Loops:                      %d\n", loopcount);
	fprintf(fout, "Messages received:          %d\n", recvcount);
	fprintf(fout, "Bytes/msg:                  %d\n", BUFFERSIZE_R);
	fprintf(fout, "Messages/second:            %f\n", (double)recvcount/s);
	fprintf(fout, "Data transferred:           %f Mb\n", (double)recvcount*BUFFERSIZE_R*8/1e6);
	fprintf(fout, "sync_xferred:               %f Mb\n", (double)sync_xferred*8/1e6);
	fprintf(fout, "Transfer speed:             %f Mb/s\n", (double)(recvcount*BUFFERSIZE_R*8)/(s*1e6));
	fprintf(fout, "========================================================\n\n");

}

static void senddbgoutput(FILE* fout, double s, int sendcount, int loopcount, int sync_xferred) {
	fprintf(fout, "========================================================\n");
	fprintf(fout, "Duration:                   %f\n", s);
	fprintf(fout, "Loops:                      %d\n", loopcount);
	fprintf(fout, "Messages sent:              %d\n", sendcount);
	fprintf(fout, "Bytes/msg:                  %d\n", BUFFERSIZE_S);
	fprintf(fout, "Messages/second:            %f\n", (double)sendcount/s);
	fprintf(fout, "Data transferred:           %f Mb\n", (double)sendcount*BUFFERSIZE_S*8/1e6);
	fprintf(fout, "sync_xferred:               %f Mb\n", (double)sync_xferred*8/1e6);
	fprintf(fout, "Transfer speed:             %f Mb/s\n", (double)(sendcount*BUFFERSIZE_S*8)/(s*1e6));
	fprintf(fout, "========================================================\n\n");
}







