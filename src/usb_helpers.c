#include "usb_helpers.h"
#include "debug.h"
#include "callbacks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define EP_IN  0x81
#define EP_OUT 0x01

bool pendingWrite = false, pendingReceive = false;

static const byte_t test_msg[] = "test";
static const int test_msg_len = 4;
static byte_t test_recv[2];

/* Local function prototype */
static int _test_connection(libusb_device_handle* dev_handle);

/* Generic function for sending data to the device spedified by dev_handle, endpoint defined by #define for convenience */
void send_async_transfer(libusb_device_handle* dev_handle, unsigned char* message, int msgSize, int timeout) {
	struct libusb_transfer* transfer = NULL;
	int rc;
	//Allocate a transfer with 0 isochronous packages
	transfer = libusb_alloc_transfer(0);
	pendingWrite = true;
	debugprint("Trying to send a message...", YELLOW);
	libusb_fill_bulk_transfer(
		transfer,
		dev_handle,
		EP_OUT,
		message,
		msgSize,
		/* TODO: Fix the callback */
		&mcu_dataSentCallback,
		NULL,
		timeout //Unsure what timeout value we should set
	);

	rc = libusb_submit_transfer(transfer);
	if (rc == 0) {
	  /* pendingWrite = true; */
	}else if (rc == LIBUSB_ERROR_NO_DEVICE) {
		debugprint("Device has disconnected!", RED);
	} else if (rc == LIBUSB_ERROR_BUSY) {
		debugprint("transfer already submitted!", YELLOW);
	} else if (rc == LIBUSB_ERROR_NOT_SUPPORTED){
		debugprint("Transfer flags not supported!", RED);
	} else {
	  // lol no
	  debugprint("Error has occured while sending ...", RED);
	}
}

/* Generic function for receiving data from the device spedified by dev_handle, endpoint defined by #define for convenience */

void recv_async_transfer(libusb_device_handle* dev_handle, unsigned char* buffer, int buflen, int timeout) {
	struct libusb_transfer* transfer = NULL;
	int rc;
	//Allocate a transfer with 0 isochronous packages
	transfer = libusb_alloc_transfer(0);
	pendingReceive = true;
	libusb_fill_bulk_transfer(
		transfer, //transfer struct pointer
		dev_handle, //device we want to receive from
		EP_IN, //endpoint we want to receive from
		buffer, //buffer we want to receive into
		buflen, //buffer length
		/* TODO: Fix the callback */
		&mcu_dataReceivedCallback,
		NULL,
		timeout //Unsure what timeout value we should set
	);

	rc = libusb_submit_transfer(transfer);

	/* TODO: Fix error handling */
	if (rc) {

	} else {
		
	}

	/* pendingReceive = true; */
}

/* Returns length of name stored in stringBuffer, if return value < 0 the name fetching failed */
/* Supplied buffer should be at least 200 bytes just to be sure */
int get_device_name(libusb_device_handle* dev_handle, char* stringBuffer, int bufferLength) {
		int rc = 0, mlength = 0, plength = 0;
		unsigned char manufacturer[100], product[100]; //Just assume names are never longer than 100 chars
		memset(manufacturer, 0, 100);
		memset(product, 0, 100);

		/* Get name of connected device */
		libusb_device* dev = libusb_get_device(dev_handle);
		struct libusb_device_descriptor desc;
		rc = libusb_get_device_descriptor(dev, &desc);
		if (rc) {
			/* TODO: Proper error handling? */
			return -1;
		}
		mlength = libusb_get_string_descriptor_ascii(dev_handle, desc.iManufacturer, manufacturer, 100);
		plength = libusb_get_string_descriptor_ascii(dev_handle, desc.iProduct, product, 100);
		memcpy(stringBuffer, manufacturer, mlength);
		memset(stringBuffer+mlength, ' ', 1);
		memcpy(stringBuffer+mlength+1, product, plength+1); //+1 in order to get the null-terminator;
		return mlength+plength+1; //+1 because of the space seperating the two strings
}


/* Test functions, only for testing purposes */

void discover_devices(libusb_context* context) {
	libusb_device** device_list = NULL;
	ssize_t count = 0;
	int rc = 0;
	
	count = libusb_get_device_list(context, &device_list);
	assert(count > 0);
	
	printf("----------------------\n");
	printf(ANSI_COLOR_BLUE " # | Vendor | Device  \n" ANSI_COLOR_RESET);
	printf("----------------------\n");

	for (size_t i = 0; i < count; i++) {
		libusb_device* device = device_list[i];
		struct libusb_device_descriptor desc;

		rc = libusb_get_device_descriptor(device, &desc);
		assert(rc == 0);

		printf(ANSI_COLOR_CYAN " %d |  %04x  |  %04x  \n" ANSI_COLOR_RESET, (int)i, desc.idVendor, desc.idProduct);
		printf("----------------------\n");
	}

	libusb_free_device_list(device_list, 1);
}

void get_device_names(libusb_context* context) {
	libusb_device** device_list = NULL;
	ssize_t count = 0;
	int rc = 0;
	unsigned char vendor[256], product[256];

	count = libusb_get_device_list(context, &device_list);
	assert(count > 0);

	for (size_t i = 0; i < count; i++) {
		libusb_device* device = device_list[i];
		struct libusb_device_descriptor desc;

		rc = libusb_get_device_descriptor(device, &desc);
		assert(rc == 0);

		libusb_device_handle* dev_handle = NULL;
		rc = libusb_open(device, &dev_handle);
		if (rc) {
			#ifdef DEBUG
			fprintf(stderr, ANSI_COLOR_RED "ERROR: Could not open device %04x:%04x\n" ANSI_COLOR_RESET, desc.idVendor, desc.idProduct);
			#endif
		} else {
			rc = libusb_get_string_descriptor_ascii(dev_handle, desc.iManufacturer, vendor, 256);
			if (rc < 0) {
				#ifdef DEBUG
				fprintf(stderr, ANSI_COLOR_RED "ERROR: Could not get Manufacturer string from device %04x:%04x\n" ANSI_COLOR_RESET, desc.idVendor, desc.idProduct);
				#endif
				memset(vendor, '\0', sizeof(vendor));
			}

			rc = libusb_get_string_descriptor_ascii(dev_handle, desc.iProduct, product, 256);
			if (rc < 0) {
				#ifdef DEBUG
				fprintf(stderr, ANSI_COLOR_RED "ERROR: Could not get Product string from device %04x:%04x\n" ANSI_COLOR_RESET, desc.idVendor, desc.idProduct);
				#endif
				memset(product, '\0', sizeof(product));
			}
		}
		printf("---------------------------------------------------\n");
		printf(ANSI_COLOR_MAGENTA "	Device: %04x:%04x\n" ANSI_COLOR_RESET, desc.idVendor, desc.idProduct);
		printf(ANSI_COLOR_MAGENTA "	Manufacturer: %s\n" ANSI_COLOR_RESET, vendor);
		printf(ANSI_COLOR_MAGENTA "	Product:      %s\n" ANSI_COLOR_RESET, product);
		printf("---------------------------------------------------\n\n");

		libusb_close(dev_handle);
	}
	libusb_free_device_list(device_list, 1);
}

int test_connection(libusb_device_handle* mcu_handle, libusb_device_handle* fpga_handle, pacman_device_t device) {
	int rc0, rc1; rc0 = rc1 = 0;
	switch (device) {
		case PACMAN_MCU_DEVICE :
			rc0 = _test_connection(mcu_handle);
			break;
		case PACMAN_FPGA_DEVICE :
			rc1 = _test_connection(fpga_handle);
			break;
		case PACMAN_BOTH_DEVICES :
			rc0 = _test_connection(mcu_handle);
			rc1 = _test_connection(fpga_handle);
			break;
		case PACMAN_NO_DEVICE :
			return 1;
	}

	return rc0 || rc1;


}

static int _test_connection(libusb_device_handle* dev_handle) {
	memset(test_recv, 0, 2);
	send_async_transfer(dev_handle, test_msg, test_msg_len, 1000);
	recv_async_transfer(dev_handle, test_recv, 2, 1000);

	/* TODO: Finish this */
} 
