#include "tusb_option.h"

#if CFG_TUD_CDC && !defined(ARDUINO_ARCH_ESP32)

#include "pico/bootrom.h"
#include "usbd_cdc.h"
#include "usbd_device.h"

#define BULK_PACKET_SIZE (TUD_OPT_HIGH_SPEED ? 512 : 64)

void yield() {
  tud_task();
  uint8_t const cdc_instance = USBD_CDC::getInstanceCount();
  for (uint8_t instance = 0; instance < cdc_instance; instance++) {
    tud_cdc_n_write_flush(instance);
  }
}

// SerialTinyUSB can be macro expanding to "Serial" on supported cores
USBD_CDC SerialTinyUSB;

uint8_t USBD_CDC::_instance_count = 0;
USBD_CDC::USBD_CDC(void) { _instance = INVALID_INSTANCE; }


USBD_CDC::~USBD_CDC()
{
}

uint16_t USBD_CDC::getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                                   uint8_t *buf,
                                                   uint16_t bufsize) {
  (void)itfnum_deprecated;

  // CDC is mostly always existed for DFU
  uint8_t itfnum = 0;
  uint8_t ep_notif = 0;
  uint8_t ep_in = 0;
  uint8_t ep_out = 0;

  if (buf) {
    itfnum = USBDevice.allocInterface(2);
    ep_notif = USBDevice.allocEndpoint(TUSB_DIR_IN);
    ep_in = USBDevice.allocEndpoint(TUSB_DIR_IN);
    ep_out = USBDevice.allocEndpoint(TUSB_DIR_OUT);
  }

#if TINYUSB_API_VERSION < 20400
  // backward compatible for core that include pre-2.4.0 TinyUSB
  uint8_t _strid = 0;
#endif

  uint8_t const desc[] = {TUD_CDC_DESCRIPTOR(itfnum, _strid, ep_notif, 8,
                                             ep_out, ep_in, BULK_PACKET_SIZE)};

  uint16_t const len = sizeof(desc);

  // null buffer is used to get the length of descriptor only
  if (buf) {
    if (bufsize < len) {
      return 0;
    }
    memcpy(buf, desc, len);
  }

  return len;
}

uint8_t USBD_CDC::setStringIndex(uint8_t index) {
  if (index >= 12)
  return 0;

  return stringIndex = index;

}

// Baud and config is ignore in CDC
void USBD_CDC::begin(uint32_t baud) {
  (void)baud;

  // already called begin()
  if (isValid()) {
    return;
  }

  // too many instances
  if (!(_instance_count < CFG_TUD_CDC)) {
    return;
  }

  _instance = _instance_count++;
  this->setStringDescriptor("TinyUSB Serial");
  USBDevice.addInterface(*this);
}

void USBD_CDC::begin(uint32_t baud, uint8_t config) {
  (void)config;
  this->begin(baud);
}

void USBD_CDC::end(void) {
  // Reset configuration descriptor without Serial as CDC
  USBDevice.clearConfiguration();
  _instance_count = 0;
  _instance = INVALID_INSTANCE;
}

uint32_t USBD_CDC::baud(void) {
  if (!isValid()) {
    return 0;
  }

  cdc_line_coding_t coding;
  tud_cdc_n_get_line_coding(_instance, &coding);

  return coding.bit_rate;
}

uint8_t USBD_CDC::stopbits(void) {
  if (!isValid()) {
    return 0;
  }

  cdc_line_coding_t coding;
  tud_cdc_n_get_line_coding(_instance, &coding);

  return coding.stop_bits;
}

uint8_t USBD_CDC::paritytype(void) {
  if (!isValid()) {
    return 0;
  }

  cdc_line_coding_t coding;
  tud_cdc_n_get_line_coding(_instance, &coding);

  return coding.parity;
}

uint8_t USBD_CDC::numbits(void) {
  if (!isValid()) {
    return 0;
  }

  cdc_line_coding_t coding;
  tud_cdc_n_get_line_coding(_instance, &coding);

  return coding.data_bits;
}

int USBD_CDC::dtr(void) {
  if (!isValid()) {
    return 0;
  }

  return tud_cdc_n_connected(_instance);
}

USBD_CDC::operator bool() {
  if (!isValid()) {
    return false;
  }

  bool ret = tud_cdc_n_connected(_instance);

  // Add an yield to run usb background in case sketch block wait as follows
  // while( !Serial ) {}
  if (!ret) {
    yield();
  }
  return ret;
}

int USBD_CDC::available(void) {
  if (!isValid()) {
    return 0;
  }

  uint32_t count = tud_cdc_n_available(_instance);

  // Add an yield to run usb background in case sketch block wait as follows
  // while( !Serial.available() ) {}
  if (!count) {
    yield();
  }

  return count;
}

int USBD_CDC::peek(void) {
  if (!isValid()) {
    return -1;
  }

  uint8_t ch;
  return tud_cdc_n_peek(_instance, &ch) ? (int)ch : -1;
}

int USBD_CDC::read(void) {
  if (!isValid()) {
    return -1;
  }
  return (int)tud_cdc_n_read_char(_instance);
}

#if TINYUSB_API_VERSION >= 10700
size_t USBD_CDC::read(uint8_t *buffer, size_t size) {
  if (!isValid()) {
    return 0;
  }

  return tud_cdc_n_read(_instance, buffer, size);
}
#endif

void USBD_CDC::flush(void) {
  if (!isValid()) {
    return;
  }

  tud_cdc_n_write_flush(_instance);
}

size_t USBD_CDC::write(uint8_t ch) { return write(&ch, 1); }

size_t USBD_CDC::write(const uint8_t *buffer, size_t size) {
  if (!isValid()) {
    return 0;
  }

  size_t remain = size;
  while (remain && tud_cdc_n_connected(_instance)) {
    size_t wrcount = tud_cdc_n_write(_instance, buffer, remain);
    remain -= wrcount;
    buffer += wrcount;

    // Write FIFO is full, run usb background to flush
    if (remain) {
      yield();
    }
  }

  return size - remain;
}

int USBD_CDC::availableForWrite(void) {
  if (!isValid()) {
    return 0;
  }
  return tud_cdc_n_write_available(_instance);
}

extern "C" {

// Invoked when cdc when line state changed e.g connected/disconnected
// Use to reset to DFU when disconnect with 1200 bps
void tud_cdc_line_state_cb(uint8_t instance, bool dtr, bool rts) {
  (void)rts;

  // DTR = false is counted as disconnected
  if (!dtr) {
    // touch1200 only with first CDC instance (Serial)
    if (instance == 0) {
      cdc_line_coding_t coding;
      tud_cdc_get_line_coding(&coding);

      if (coding.bit_rate == 1200) {
        reset_usb_boot(0, 0);
          while (1) {
        }
      }
    }
  }
}
}

#else

// Device stack is not enabled (probably in host mode)
#warning "TinyUSB Host selected. No output to Serial will occur!"

uint16_t USBD_CDC::getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                                   uint8_t *buf,
                                                   uint16_t bufsize) {
  (void)itfnum_deprecated;
  (void)buf;
  (void)bufsize;

  return 0;
}

// Baud and config is ignore in CDC
void USBD_CDC::begin(uint32_t baud) { (void)baud; }

void USBD_CDC::begin(uint32_t baud, uint8_t config) { (void)config; }

void USBD_CDC::end(void) {}

uint32_t USBD_CDC::baud(void) { return 0; }

uint8_t USBD_CDC::stopbits(void) { return 0; }

uint8_t USBD_CDC::paritytype(void) { return 0; }

uint8_t USBD_CDC::numbits(void) { return 0; }

int USBD_CDC::dtr(void) { return 0; }

USBD_CDC::operator bool() { return false; }

int USBD_CDC::available(void) { return 0; }

int USBD_CDC::peek(void) { return -1; }

int USBD_CDC::read(void) { return -1; }

size_t USBD_CDC::read(uint8_t *buffer, size_t size) {
  (void)buffer;
  (void)size;
  return 0;
}

void USBD_CDC::flush(void) {}

size_t USBD_CDC::write(uint8_t ch) { return -1; }

size_t USBD_CDC::write(const uint8_t *buffer, size_t size) {
  return 0;
}

int USBD_CDC::availableForWrite(void) { return 0; }
#endif