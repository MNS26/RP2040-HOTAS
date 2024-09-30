#ifndef USBD_MTP_H_
#define USBD_MTP_H_

#include "usbd_device.h"
#include "usbd_interface.h"
// TODO: make a propper enum to put them in
#define MTP_SUBCLASS_UNKNOWN 0xff 
#define MTP_PROTOCOL_UNKNOW 0x00
#define TUSB_INTERUPT 3

// Interface number, string index, EP Out & EP In address, EP size
#define TUD_MTP_DESCRIPTOR(_itfnum, _stridx, _epout, _epin, _ep_int, _epsize, _ep_interval) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 3, TUSB_CLASS_IMAGE, 1, 1, _stridx,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0, \
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0, \
  /* Endpoint In*/\
  7, TUSB_DESC_ENDPOINT, _ep_int, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_epsize), _ep_interval

class USBD_MTP : public USBD_Interface
{
public:
  typedef int32_t (*read_callback_t)(uint32_t lba, void *buffer,
                                     uint32_t bufsize);
  typedef int32_t (*write_callback_t)(uint32_t lba, uint8_t *buffer,
                                      uint32_t bufsize);
  typedef void (*flush_callback_t)(void);
  typedef bool (*ready_callback_t)(void);
  typedef bool (*writable_callback_t)(void);
  typedef bool (*start_stop_callback_t)(uint8_t power_condition, bool start,
                                        bool load_eject);

  USBD_MTP(void);
  ~USBD_MTP();
  bool isValid(void) { return _instance != INVALID_INSTANCE; }
  uint8_t setStringIndex(uint8_t index);
  // from Adafruit_USBD_Interface
  virtual uint16_t getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                          uint8_t *buf, uint16_t bufsize);
private:
  uint8_t stringIndex = _strid;
  enum { INVALID_INSTANCE = 0xffu };
  static uint8_t _instance_count;

  uint8_t _instance;
  uint8_t _interval_ms = 6;// i dont know if this should be changeble or not
};
#endif