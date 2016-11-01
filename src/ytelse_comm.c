#include "ytelse_comm.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Device ID
#define VENDOR_ID 0x10c4
#define PRODUCT_ID 0x0007


int get_ytelse_mcu_handle(libusb_context* context, libusb_device_handle** dev_handle) {
	libusb_device** device_list = NULL;
	libusb_device* efm_dev = NULL;
	ssize_t count = 0;
	int rc = 0;
	
	count = libusb_get_device_list(context, &device_list);
	if (count < 0) {
		colorprint("FATAL ERROR: USB Device list has fewer than 0 entries!", RED);
		return YTELSE_DEVICE_NOT_FOUND;
	}

	for (size_t i = 0; i < count; i++) {
		libusb_device* device = device_list[i];
		struct libusb_device_descriptor desc;

		rc = libusb_get_device_descriptor(device, &desc);
		if (rc) {continue;} //Fail quitely, we will keep trying from main loop

		if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID) {
			efm_dev = device;
		}

	}

	if (!efm_dev) {
		debugprint("Could not find Ytelse USB device.", RED);
		return YTELSE_DEVICE_NOT_FOUND;
	}
	
	rc = libusb_open(efm_dev, dev_handle);
	if (rc) {
		libusb_free_device_list(device_list, 1);
		debugprint("Failed to open Ytelse USB device!", RED);
		return YTELSE_DEVICE_USB_OPEN_FAILURE;
	}

	libusb_free_device_list(device_list, 1);
	return YTELSE_DEVICE_USB_OPEN_SUCCESS;
}
