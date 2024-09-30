#ifndef USBD_INTERFACE_H_
#define USBD_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

class USBD_Interface
{
protected:
    uint8_t _strid;
private:
    /* data */
public:
  USBD_Interface(void){_strid = 0;};
  ~USBD_Interface();
  // Get Interface Descriptor
  // Fill the descriptor (if buf is not NULL) and return its length
  virtual uint16_t getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                          uint8_t *buf, uint16_t bufsize) = 0;
  // Get Interface Descriptor Length
  uint16_t getInterfaceDescriptorLen() {
    return getInterfaceDescriptor(0, NULL, 0);
  }

  void setStringDescriptor(const char *str);
};
#endif