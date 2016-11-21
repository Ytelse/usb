#include "debug.h"
#include "callbacks.h"

/* Generic callbacks for MCU transfers */
int buffer_counter = 0;

void LIBUSB_CALL mcu_dataReceivedCallback(struct libusb_transfer* transfer) {
	debugprint("Data received!", RED);
	/* There are 4096 bytes, we store them in 1 byte increments*/
	for(size_t i = 0; i < 4096; i++) {
	  result_buffer[buffer_counter++] = (unsigned char) transfer->buffer[i];
	}
	libusb_free_transfer(transfer);
	pendingReceive = false;
}

void LIBUSB_CALL mcu_dataSentCallback(struct libusb_transfer* transfer) {
	libusb_free_transfer(transfer);
	pendingWrite = false;
}

/* Test callback(s) */
void LIBUSB_CALL mcu_tickWriteFinishedCallback(struct libusb_transfer* transfer) {
	libusb_free_transfer(transfer);
	pendingWrite = false;
}
