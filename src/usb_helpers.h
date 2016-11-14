#ifndef __USB_HELPERS_H_
#define __USB_HELPERS_H_

#include <libusb.h>
#include "defs.h"

/* Simple function that prints all USB devices attached to system */
void discover_devices(libusb_context* context);
/* Function that requests and prints names from all USB devices attached to the system */
void get_device_names(libusb_context* context);

/* Get device name from usb device handle, store it in stringBuffer */
int get_device_name(libusb_device_handle* dev_handle, char* stringBuffer, int bufferLength);
/* Send message buffer to device (denoted by handle) using bulk transfer */
void send_async_transfer(libusb_device_handle* dev_handle, unsigned char* message, int msgSize, int timeout);
/* Receive data from device into buffer using bulk transfer */
void recv_async_transfer(libusb_device_handle* dev_handle, unsigned char* buffer, int buflen, int timeout);

int test_connection(libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, pacman_device_t device);

#endif /* __USB_HELPERS_H_ */