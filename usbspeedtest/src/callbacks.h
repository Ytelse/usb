#ifndef __CALLBACKS_H_
#define __CALLBACKS_H_

#include <libusb.h>
#include <stdbool.h>

extern bool pending_recv, pending_write;

void LIBUSB_CALL data_received_callback(struct libusb_transfer* transfer);
void LIBUSB_CALL data_sent_callback(struct libusb_transfer* transfer);

#endif