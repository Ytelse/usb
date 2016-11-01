#include "debug.h"
#include "callbacks.h"

/* Generic callbacks for MCU transfers */

void LIBUSB_CALL mcu_dataReceivedCallback(struct libusb_transfer* transfer) {
	debugprint("Received data from device!", MAGENTA);
	libusb_free_transfer(transfer);
	pendingReceive = false;
}

void LIBUSB_CALL mcu_dataSentCallback(struct libusb_transfer* transfer) {
	debugprint("Sent data to device!", CYAN);
	libusb_free_transfer(transfer);
	pendingWrite = false;
}

/* Test callback(s) */
void LIBUSB_CALL mcu_tickWriteFinishedCallback(struct libusb_transfer* transfer) {
	debugprint("Tick write finished!", CYAN);
	libusb_free_transfer(transfer);
	pendingWrite = false;
}