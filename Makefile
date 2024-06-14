descriptor_test: desktop-descriptor.c include/HID_descriptor.h include/hid_minimal.h
	gcc desktop-descriptor.c -o $@ -Iinclude/ -I'.pio/libdeps/usb/Adafruit TinyUSB Library/src'
	./$@
