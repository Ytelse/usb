#include "debug.h"
#include "callbacks.h"

/* Generic callbacks for MCU transfers */

void LIBUSB_CALL mcu_dataReceivedCallback(struct libusb_transfer* transfer) {
	debugprint("Data received!", RED);
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