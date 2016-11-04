#include "ytelse_comm.h"
#include "usb_helpers.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Device ID
#define MCU_VENDOR_ID 0x10c4
#define MCU_PRODUCT_ID 0x0007

#define FPGA_VENDOR_ID 0x0000
#define FPGA_PRODUCT_ID 0x0000

/* Number of attempts before giving up trying to connect to device */
#define ATTEMPT_LIMIT 10

typedef enum ConnectionState {
	USB_DEVICE_CONNECTED,				/* USB device has connected */
	USB_DEVICE_DISCONNECTED,			/* USB device has disconnected */
	USB_DEVICE_FOUND,					/* USB device found */
	USB_DEVICE_NOT_FOUND,				/* USB device not found */
	USB_DEVICE_INTERFACE_CLAIMED,		/* USB interface claimed */
	USB_DEVICE_INTERFACE_BUSY,			/* USB interface busy (claimed by other process) */
	USB_DEVICE_INTERFACE_NOT_FOUND,		/* USB interface not found */
} con_state_t;

/* Internal error codes */
enum {YTELSE_DEVICE_USB_OPEN_SUCCESS, YTELSE_DEVICE_NOT_FOUND, YTELSE_DEVICE_USB_OPEN_FAILURE};

/* Function prototypes */
int get_ytelse_mcu_handle(libusb_context* context, libusb_device_handle** dev_handle);
int get_ytelse_fpga_handle(libusb_context* context, libusb_device_handle** dev_handle);



int connect(libusb_context* context, libusb_device_handle** dev_handle, ytelse_device_t device, int* interface) {
	con_state_t state = USB_DEVICE_NOT_FOUND;
	int rc = 0;
	*interface = 0;
	int attempts = 0;

	debugprint("Establishing connection to device...", DEFAULT);
	
	while (state != USB_DEVICE_FOUND) {

		/* Attempt to connect to device */
		if (device == MCU_DEVICE) {
			rc = get_ytelse_mcu_handle(context, dev_handle);
		} else {
			rc = get_ytelse_fpga_handle(context, dev_handle);
		}

		if (rc) {
			/* Keep trying until we reach attempt limit */
			if (++attempts == ATTEMPT_LIMIT) {
				break;
			}
		} else {
			state = USB_DEVICE_FOUND;
		}
	}

	/* If device is not found, return failure */
	if (state != USB_DEVICE_FOUND) {
		colorprint("ERROR: Failed to get USB device handle!", RED);
		return CONNECT_FAILURE;
	}

	debugprint("Successfully got device handle!", GREEN);


	debugprint("Claiming USB device interface...", DEFAULT);

	/* Claim a interface on the USB device */
	while (state != USB_DEVICE_INTERFACE_CLAIMED) {
		
		rc = libusb_claim_interface(*dev_handle, *interface);

		if (rc == LIBUSB_ERROR_NOT_FOUND) {
			/* Interface not found on current device. Should not happen. */
			debugprint("WARNING: Interface not found! Trying another...", YELLOW);
			state = USB_DEVICE_INTERFACE_NOT_FOUND;
			if (++*interface == ATTEMPT_LIMIT) {
				/* Only keep trying new interfaces up to the attempt limit */
				break;
			}
		} else if (rc == LIBUSB_ERROR_BUSY) {
			debugprint("WARNING: Device busy, trying again in 1 second...", YELLOW);
			state = USB_DEVICE_INTERFACE_BUSY;
			sleep(1);
		} else if (rc == LIBUSB_ERROR_NO_DEVICE) {
			/* Device has disconnected before we were able to claim the interface. */
			debugprint("ERROR: Device disconnected before interface was claimed!", RED);
			break;
		} else {
			state = USB_DEVICE_INTERFACE_CLAIMED;
		}
	}

	if (state != USB_DEVICE_INTERFACE_CLAIMED) {
		colorprint("ERROR: Failed to claim USB device interface!", RED);
		return CONNECT_FAILURE;
	} else {
		state = USB_DEVICE_CONNECTED;
	}
	
	debugprint("Successfully claimed USB device interface!", GREEN);

	char nameBuffer[200];
	int nameLength = getDeviceName(*dev_handle, nameBuffer, 200);
	if (nameLength < 0) {
		colorprint("WARNING: Failed to get device name.", YELLOW);
	} else {
		colorprint("Connected to:", BLUE);
		colorprint(nameBuffer, BLUE);
	}

	colorprint("Connection established!", GREEN);

	return CONNECT_SUCCESS;
}

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

		if (desc.idVendor == MCU_VENDOR_ID && desc.idProduct == MCU_PRODUCT_ID) {
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

int get_ytelse_fpga_handle(libusb_context* context, libusb_device_handle** dev_handle) {
	/* PLACEHOLDE */
	return YTELSE_DEVICE_NOT_FOUND;
}
