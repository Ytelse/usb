#include "debug.h"
#include "callbacks.h"

/* Generic callbacks for MCU transfers */
int buffer_counter = 0;

void LIBUSB_CALL mcu_dataReceivedCallback(struct libusb_transfer* transfer) {
	debugprint("Data received!", RED);
	result_buffer[buffer_counter++] = (int) transfer->buffer;
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
