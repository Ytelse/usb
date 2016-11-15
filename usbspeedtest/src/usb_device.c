#include "usb_device.h"

#include <stdio.h>
#include <string.h>

#define UNUSED(x) (void) x

static int getDeviceName(libusb_device_handle* dev_handle, char* stringBuffer, int bufferLength);

int connect(libusb_context* context, libusb_device_handle** dev_handle, int interface) {
	libusb_device** device_list = NULL;
	libusb_device* efm_dev = NULL;

	ssize_t count = 0;
	count = libusb_get_device_list(context, &device_list);

	for (size_t i = 0; i < count; i++) {
		libusb_device* device = device_list[i];
		struct libusb_device_descriptor desc;
		libusb_get_device_descriptor(device, &desc);

		if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID) {
			efm_dev = device;
		}
	}

	if (!efm_dev) {
		return 1;
	}

	if(libusb_open(efm_dev, dev_handle)) {
		libusb_free_device_list(device_list, 1);
		return 1;
	}

	libusb_free_device_list(device_list, 1);

	if(libusb_claim_interface(*dev_handle, interface)) {
		return 1;
	}

	char nameBuffer[256];
	int nameLength = getDeviceName(*dev_handle, nameBuffer, 256);
	UNUSED(nameLength);
	printf("Connected to %s\n", nameBuffer);

	return 0;
}

static int getDeviceName(libusb_device_handle* dev_handle, char* stringBuffer, int bufferLength) {
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