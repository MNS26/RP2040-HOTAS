#include "tusb_option.h"
#include "usbd_hid.h"
uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};
static USBD_HID *_hid_instances[CFG_TUD_HID] = {0};

uint8_t USBD_HID::_instance_count = 0;

//------------- IMPLEMENTATION -------------//
USBD_HID::USBD_HID(void)
    : USBD_HID(NULL, 0, HID_ITF_PROTOCOL_NONE, 4, false) {}

USBD_HID::USBD_HID(uint8_t const *desc_report, uint16_t len,
                                     uint8_t protocol, uint8_t interval_ms,
                                     bool has_out_endpoint) {
  _instance = INVALID_INSTANCE;
  _interval_ms = interval_ms;
  _protocol = protocol;

  _out_endpoint = has_out_endpoint;
  _mouse_button = 0;

  _desc_report = desc_report;
  _desc_report_len = len;

  _get_report_cb = NULL;
  _set_report_cb = NULL;
}

USBD_HID::~USBD_HID()
{
}


void USBD_HID::setPollInterval(uint8_t interval_ms) {
  _interval_ms = interval_ms;
}

void USBD_HID::setBootProtocol(uint8_t protocol) {
  _protocol = protocol;
}

bool USBD_HID::isOutEndpointEnabled(void) { return _out_endpoint; }

void USBD_HID::enableOutEndpoint(bool enable) {
  _out_endpoint = enable;
}

void USBD_HID::setReportDescriptor(uint8_t const *desc_report,
                                            uint16_t len) {
  _desc_report = desc_report;
  _desc_report_len = len;
}

void USBD_HID::setReportCallback(get_report_callback_t get_report,
                                          set_report_callback_t set_report) {
  _get_report_cb = get_report;
  _set_report_cb = set_report;
}

uint16_t USBD_HID::makeItfDesc(uint8_t itfnum, uint8_t *buf,
                                        uint16_t bufsize, uint8_t ep_in,
                                        uint8_t ep_out) {
  if (!_desc_report_len) {
    return 0;
  }

  uint8_t const desc_inout[] = {TUD_HID_INOUT_DESCRIPTOR(
      itfnum, _strid, _protocol, _desc_report_len, ep_in, ep_out,
      CFG_TUD_HID_EP_BUFSIZE, _interval_ms)};
  uint8_t const desc_in_only[] = {
      TUD_HID_DESCRIPTOR(itfnum, _strid, _protocol, _desc_report_len, ep_in,
                         CFG_TUD_HID_EP_BUFSIZE, _interval_ms)};

  uint8_t const *desc;
  uint16_t len;

  if (_out_endpoint) {
    desc = desc_inout;
    len = sizeof(desc_inout);
  } else {
    desc = desc_in_only;
    len = sizeof(desc_in_only);
  }

  // null buffer is used to get the length of descriptor only
  if (buf) {
    if (bufsize < len) {
      return 0;
    }
    memcpy(buf, desc, len);
  }

  return len;
}

uint16_t USBD_HID::getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                                   uint8_t *buf,
                                                   uint16_t bufsize) {
  (void)itfnum_deprecated;

  uint8_t itfnum = 0;
  uint8_t ep_in = 0;
  uint8_t ep_out = 0;

  // null buffer is used to get the length of descriptor only
  if (buf) {
    itfnum = USBDevice.allocInterface(1);
    ep_in = USBDevice.allocEndpoint(TUSB_DIR_IN);

    if (_out_endpoint) {
      ep_out = USBDevice.allocEndpoint(TUSB_DIR_OUT);
    }
  }

  return makeItfDesc(itfnum, buf, bufsize, ep_in, ep_out);
}

uint8_t USBD_HID::setStringIndex(uint8_t index) {
  if (index >= 12)
  return 0;

  return stringIndex = index;

}
bool USBD_HID::begin(void) {
  if (isValid()) {
    return true;
  }

  if (_instance_count >= CFG_TUD_HID) {
    return false;
  }

  if (!USBDevice.addInterface(*this)) {
    return false;
  }
  _instance = _instance_count++;
  _hid_instances[_instance] = this;

  return true;
}

bool USBD_HID::ready(void) { return tud_hid_n_ready(_instance); }

bool USBD_HID::sendReport(uint8_t report_id, void const *report,
                                   uint8_t len) {
  return tud_hid_n_report(_instance, report_id, report, len);
}

bool USBD_HID::sendReport8(uint8_t report_id, uint8_t num) {
  return tud_hid_n_report(_instance, report_id, &num, sizeof(num));
}

bool USBD_HID::sendReport16(uint8_t report_id, uint16_t num) {
  return tud_hid_n_report(_instance, report_id, &num, sizeof(num));
}

bool USBD_HID::sendReport32(uint8_t report_id, uint32_t num) {
  return tud_hid_n_report(_instance, report_id, &num, sizeof(num));
}

uint8_t USBD_HID::getProtocol() {
  return tud_hid_n_get_protocol(_instance);
}

//------------- TinyUSB callbacks -------------//
extern "C" {

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
  USBD_HID *p_hid = _hid_instances[itf];

  if (!p_hid) {
    return NULL;
  }

  return p_hid->_desc_report;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  USBD_HID *p_hid = _hid_instances[itf];

  if (!(p_hid && p_hid->_get_report_cb)) {
    return 0;
  }

  return p_hid->_get_report_cb(report_id, report_type, buffer, reqlen);
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  USBD_HID *p_hid = _hid_instances[itf];

  if (!(p_hid && p_hid->_set_report_cb)) {
    return;
  }

  p_hid->_set_report_cb(report_id, report_type, buffer, bufsize);
}

} // extern "C"

//--------------------------------------------------------------------+
// Keyboard
//--------------------------------------------------------------------+

bool USBD_HID::keyboardReport(uint8_t report_id, uint8_t modifier,
                                       uint8_t keycode[6]) {
  return tud_hid_n_keyboard_report(_instance, report_id, modifier, keycode);
}

bool USBD_HID::keyboardPress(uint8_t report_id, char ch) {
  uint8_t keycode[6] = {0};
  uint8_t modifier = 0;
  uint8_t uch = (uint8_t)ch;

  if (_ascii2keycode[uch][0]) {
    modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
  }
  keycode[0] = _ascii2keycode[uch][1];

  return tud_hid_n_keyboard_report(_instance, report_id, modifier, keycode);
}

bool USBD_HID::keyboardRelease(uint8_t report_id) {
  return tud_hid_n_keyboard_report(_instance, report_id, 0, NULL);
}

//--------------------------------------------------------------------+
// Mouse
//--------------------------------------------------------------------+

bool USBD_HID::mouseReport(uint8_t report_id, uint8_t buttons,
                                    int8_t x, int8_t y, int8_t vertical,
                                    int8_t horizontal) {
  // cache mouse button for other API such as move, scroll
  _mouse_button = buttons;

  return tud_hid_n_mouse_report(_instance, report_id, buttons, x, y, vertical,
                                horizontal);
}

bool USBD_HID::mouseMove(uint8_t report_id, int8_t x, int8_t y) {
  return tud_hid_n_mouse_report(_instance, report_id, _mouse_button, x, y, 0,
                                0);
}

bool USBD_HID::mouseScroll(uint8_t report_id, int8_t scroll,
                                    int8_t pan) {
  return tud_hid_n_mouse_report(_instance, report_id, _mouse_button, 0, 0,
                                scroll, pan);
}

bool USBD_HID::mouseButtonPress(uint8_t report_id, uint8_t buttons) {
  return tud_hid_n_mouse_report(_instance, report_id, buttons, 0, 0, 0, 0);
}

bool USBD_HID::mouseButtonRelease(uint8_t report_id) {
  return tud_hid_n_mouse_report(_instance, report_id, 0, 0, 0, 0, 0);
}
