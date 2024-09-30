#include "usbd_device.h"
#if CFG_TUD_ENABLED

//USBD_Interface::USBD_Interface(/* args */)
//{
//}

USBD_Interface::~USBD_Interface()
{
}

void USBD_Interface::setStringDescriptor(const char *str) {
  _strid = USBDevice.addStringDescriptor(str);
}
#endif