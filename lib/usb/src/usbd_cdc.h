#ifndef USBD_CDC_H_
#define USBD_CDC_H_



#include "usbd_interface.h"
#include <string.h>

class USBD_CDC: public USBD_Interface
{
public:
  USBD_CDC(void);
  ~USBD_CDC();

  static uint8_t getInstanceCount(void) { return _instance_count; }

  void setPins(uint8_t pin_rx, uint8_t pin_tx) {
    (void)pin_rx;
    (void)pin_tx;
  }
  void begin(uint32_t baud);
  void begin(uint32_t baud, uint8_t config);
  void end(void);

  // return line coding set by host
  uint32_t baud(void);
  uint8_t stopbits(void);
  uint8_t paritytype(void);
  uint8_t numbits(void);
  int dtr(void);

  // Stream API
  virtual int available(void);
  virtual int peek(void);

  virtual int read(void);
  size_t read(uint8_t *buffer, size_t size);

  virtual void flush(void);
  virtual size_t write(uint8_t);

  size_t write(const char *str) {
    if (str == NULL) return 0;
    return write((const uint8_t *)str, strlen(str));
  }
  virtual size_t write(const uint8_t *buffer, size_t size);
  size_t write(const char *buffer, size_t size) {
    return write((const uint8_t *)buffer, size);
  }

  virtual int availableForWrite(void);
  size_t print(const char* _string) {size_t n = write(_string); flush(); return n;}
  size_t print(uint64_t num) {size_t n = write(num); flush(); return n;}
  
  size_t println() {size_t n = write("\r\n"); flush(); return n;}
  size_t println(const char* _string) {size_t n = write(_string); n += println(); flush(); return n;}
  size_t println(uint64_t num) {size_t n = write(num); n += println(); flush(); return n;}
  
  
  
  //using Print::write; // pull in write(str) from Print
  operator bool();

  // from Adafruit_USBD_Interface
  virtual uint16_t getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                          uint8_t *buf, uint16_t bufsize);

  uint8_t setStringIndex(uint8_t index);

  bool isValid(void) { return _instance != INVALID_INSTANCE; }

private:
  enum { INVALID_INSTANCE = 0xffu };
  static uint8_t _instance_count;

  uint8_t _instance;

  uint8_t stringIndex = 0;

};


#endif