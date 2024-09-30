#ifndef USBD_DEVICE_H_
#define USBD_DEVICE_H_

#include "usbd_interface.h"
#include "tusb.h"
class USBD_Device
{
private:
  enum { STRING_DESCRIPTOR_MAX = 12 };

  // Configuration descripotr
  tusb_desc_device_t _desc_device __attribute__((aligned(4)));

  // Configuration descriptor
  uint8_t *_desc_cfg;
  uint8_t _desc_cfg_buffer[256];
  uint16_t _desc_cfg_len;
  uint16_t _desc_cfg_maxlen;

  uint8_t _itf_count;

  uint8_t _epin_count;
  uint8_t _epout_count;

  // String descriptoir
  const char *_desc_str_arr[STRING_DESCRIPTOR_MAX];
  uint8_t _desc_str_count;
  uint16_t _desc_str[32 + 1]; // up to 32 unicode characters with headers


public:
  USBD_Device(/* args */);
  ~USBD_Device();
  void setID(uint16_t vid, uint16_t pid);
  void setVersion(uint16_t bcd);
  void setDeviceVersion(uint16_t bcd);
  void setLanguageDescriptor(uint16_t language_id);
  void setManufacturerDescriptor(const char *s);
  void setProductDescriptor(const char *s);
  void setSerialDescriptor(const char *s);
  uint8_t addStringDescriptor(const char *s);
  bool addInterface(USBD_Interface &itf);

  void clearConfiguration(void);

    // Provide user buffer for configuration descriptor, if total length > 256
  void setConfigurationBuffer(uint8_t *buf, uint32_t buflen);

  // Allocate a new interface number
  uint8_t allocInterface(uint8_t count = 1) {
    uint8_t ret = _itf_count;
    _itf_count += count;
    return ret;
  }

  uint8_t allocEndpoint(uint8_t in) {
    uint8_t ret = in ? (0x80 | _epin_count++) : _epout_count++;
    return ret;
  }

  uint8_t getSerialDescriptor(uint16_t *serial_utf16);

//------------- Control -------------//

  bool begin(uint32_t rhport = 0);
  bool isInitialized(uint8_t rhport = 0);
  void task(void);

  // physical disable/enable pull-up
  bool detach(void);
  bool attach(void);

  //------------- status -------------//
  bool mounted(void);
  bool suspended(void);
  bool ready(void);
  bool remoteWakeup(void);




private:
  uint16_t const *descriptor_string_cb(uint8_t index, uint16_t langid);

  friend uint8_t const *tud_descriptor_device_cb(void);
  friend uint8_t const *tud_descriptor_configuration_cb(uint8_t index);
  friend uint16_t const *tud_descriptor_string_cb(uint8_t index,
                                                  uint16_t langid);
};

extern USBD_Device USBDevice;

#endif