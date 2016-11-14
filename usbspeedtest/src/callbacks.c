#include "callbacks.h"

void LIBUSB_CALL data_received_callback(struct libusb_transfer* transfer) {
	libusb_free_transfer(transfer);
	pending_recv = false;
}

void LIBUSB_CALL data_sent_callback(struct libusb_transfer* transfer) {
	libusb_free_transfer(transfer);
	pending_write = false;
}