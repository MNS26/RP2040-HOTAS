#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_
enum
{
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_JOYSTICK,
  REPORT_ID_COUNT
};
enum { 
  STRID_LANGUAGE = 0, 
  STRID_MANUFACTURER, 
  STRID_PRODUCT, 
  STRID_SERIAL 
};
enum { 
  STRING_DESCRIPTOR_MAX = 12 
};

enum
{
#if CFG_TUD_CDC
  ITF_NUM_CDC = 0,
  ITF_NUM_CDC_DATA,
#endif
#if CFG_TUD_PTP
  ITF_NUM_PTP,
#endif
#if CFG_TUD_HID
  ITF_NUM_HID,
#endif
  ITF_NUM_TOTAL
};

enum {
#if CFG_TUD_CDC
  EPNUM_CDC_NOTIF = 1,
  EPNUM_CDC,
#endif
#if CFG_TUD_PTP
  EPNUM_PTP_NOTIF,
  EPNUM_PTP,
#endif
#if CFG_TUD_HID
  EPNUM_HID,
#endif
  EPNUM_TOTAL,
};

extern uint8_t desc_config[];
extern uint16_t usb_hid_descriptor_length;
extern uint8_t usb_hid_descriptor[];
#endif /* USB_DESCRIPTORS_H_ */
