#include "debug.h"
#include "callbacks.h"
#include <stdio.h>
#include <stdlib.h>

/* Generic callbacks for MCU transfers */
int buffer_counter = 0;

void LIBUSB_CALL mcu_dataReceivedCallback(struct libusb_transfer* transfer) {
	if(transfer->status == LIBUSB_TRANSFER_COMPLETED) {
	  /* debugprint("Data received!", GREEN); */
	  /* There are 4096 bytes, we store them in 1 byte increments*/
	  for(size_t i = 0; i < 4096*4; i++) {
	    result_buffer[i] = (unsigned char) transfer->buffer[i];
	  }
	  FILE* f = fopen("out_result", "ab");
	  if(!f) {
	    printf("ERROR: Failed to open output file\n");
	    exit(1);
	  }
	  fwrite(result_buffer, 1, 4096*4, f);
	  fclose(f);
	} else {
	  debugprint("A TRANSFER FROM MCU TO HOST HAS FAILED!", RED);
	}
	
	libusb_free_transfer(transfer);
	pendingReceive = false;
}

void LIBUSB_CALL mcu_dataSentCallback(struct libusb_transfer* transfer) {
  if(transfer->status == LIBUSB_TRANSFER_COMPLETED) {
    debugprint("Data sent!", GREEN);
  } else {
    debugprint("Data not sent!", RED);
  }
	libusb_free_transfer(transfer);
	pendingWrite = false;
}

/* Test callback(s) */
void LIBUSB_CALL mcu_tickWriteFinishedCallback(struct libusb_transfer* transfer) {
	libusb_free_transfer(transfer);
	pendingWrite = false;
}
