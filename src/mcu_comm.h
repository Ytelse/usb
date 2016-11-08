#ifndef __MCU_COMM_H_
#define __MCU_COMM_H_

/* Pass a pointer to a struct containing all necessary data for the communication */
void* mcu_runloop(void* usb_device_struct_void_ptr);


#endif /* __MCU_COMM_H_ */

