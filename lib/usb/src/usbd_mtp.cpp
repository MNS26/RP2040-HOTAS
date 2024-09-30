#include "tusb_option.h"
#if CFG_TUD_ENABLED && CFG_TUD_MSC

#include "usbd_mtp.h"

#define BULK_PACKET_SIZE (TUD_OPT_HIGH_SPEED ? 512 : 64)

USBD_MTP::USBD_MTP(/* args */)
{
}

USBD_MTP::~USBD_MTP()
{
}

uint8_t USBD_MTP::setStringIndex(uint8_t index) {
  if (index >= 12)
  return 0;

  return stringIndex = index;

}
uint16_t USBD_MTP::getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                                   uint8_t *buf,
                                                   uint16_t bufsize) {
  (void)itfnum_deprecated;

  // null buffer is used to get the length of descriptor only
  if (!buf) {
    return TUD_MSC_DESC_LEN;
  }

  uint8_t const itfnum = USBDevice.allocInterface(3);
  uint8_t const ep_in = USBDevice.allocEndpoint(TUSB_DIR_IN);
  uint8_t const ep_out = USBDevice.allocEndpoint(TUSB_DIR_OUT);
  uint8_t const ep_in_int = USBDevice.allocEndpoint(TUSB_INTERUPT);

  uint8_t const desc[] = {
    TUD_MTP_DESCRIPTOR(itfnum, stringIndex, ep_out, ep_in, ep_in_int, BULK_PACKET_SIZE, _interval_ms)
};
  uint16_t const len = sizeof(desc);

  if (bufsize < len) {
    return 0;
  }
  memcpy(buf, desc, len);

  return len;
}
#endif