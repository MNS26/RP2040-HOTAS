# WIP
you will need to add your self to plugdev group on linux (if not there make it)

you will also need to add a new usb rule to the udev rules file `SUBSYSTEM=="hidraw", GROUP="plugdev"`
this is to allow users part of the plugdev group to access the HID device without having to use root
