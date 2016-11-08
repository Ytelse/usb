#include "defs.h"
#include "debug.h"
#include "pthread_helper.h"
#include "test_usb_functs.h"

#include <pthread.h>
#include <stdio.h>

#define OUTPUT_FP "resources/mcu_output.txt"

extern int _keepalive;
extern barrier_t barrier;



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


	while(_keepalive) {
		/* TODO: Receive packets and write to output file */
		sendNTicks(pdata->context, pdata->dev_handle, 1);
	};

	debugprint("MCU_comm: Finished busy-wait. Closing open file.", DEFAULT);

	fclose(f);

	return NULL;
}