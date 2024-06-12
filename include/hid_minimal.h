#pragma once
enum {
  HID_USAGE_PAGE_UNDEFINED       = 0x00,
  HID_USAGE_PAGE_DESKTOP         = 0x01,
  HID_USAGE_PAGE_SIMULATE        = 0x02,
  HID_USAGE_PAGE_VIRTUAL_REALITY = 0x03,
  HID_USAGE_PAGE_SPORT           = 0x04,
  HID_USAGE_PAGE_GAME            = 0x05,
  HID_USAGE_PAGE_GENERIC_DEVICE  = 0x06,
  HID_USAGE_PAGE_KEYBOARD        = 0x07,
  HID_USAGE_PAGE_LED             = 0x08,
  HID_USAGE_PAGE_BUTTON          = 0x09,
  HID_USAGE_PAGE_ORDINAL         = 0x0a,
  HID_USAGE_PAGE_TELEPHONY       = 0x0b,
  HID_USAGE_PAGE_CONSUMER        = 0x0c,
  HID_USAGE_PAGE_DIGITIZER       = 0x0d,
  HID_USAGE_PAGE_PID             = 0x0f,
  HID_USAGE_PAGE_UNICODE         = 0x10,
  HID_USAGE_PAGE_ALPHA_DISPLAY   = 0x14,
  HID_USAGE_PAGE_MEDICAL         = 0x40,
  HID_USAGE_PAGE_MONITOR         = 0x80,
  //0x81 - 0x83 n/a
  HID_USAGE_PAGE_POWER           = 0x84,
  //0x085 - 0x8b n/a
  HID_USAGE_PAGE_BARCODE_SCANNER = 0x8c,
  HID_USAGE_PAGE_SCALE           = 0x8d,
  HID_USAGE_PAGE_MSR             = 0x8e,
  HID_USAGE_PAGE_CAMERA          = 0x90,
  HID_USAGE_PAGE_ARCADE          = 0x91,
  HID_USAGE_PAGE_VENDOR          = 0xff
  //0xFF - 0xFFFF n/a
};

enum {
  HID_COLLECTION_PHYSICAL = 0,
  HID_COLLECTION_APPLICATION,
  HID_COLLECTION_LOGICAL,
  HID_COLLECTION_REPORT,
  HID_COLLECTION_NAMED_ARRAY,
  HID_COLLECTION_USAGE_SWITCH,
  HID_COLLECTION_USAGE_MODIFIER
};

enum {
  HID_USAGE_DESKTOP_X              = 0x30,
  HID_USAGE_DESKTOP_Y              = 0x31,
  HID_USAGE_DESKTOP_Z              = 0x32,
  HID_USAGE_DESKTOP_RX             = 0x33,
  HID_USAGE_DESKTOP_RY             = 0x34,
  HID_USAGE_DESKTOP_RZ             = 0x35,
  HID_USAGE_DESKTOP_SLIDER         = 0x36,
  HID_USAGE_DESKTOP_DIAL           = 0x37,
  HID_USAGE_DESKTOP_WHEEL          = 0x38,
  HID_USAGE_DESKTOP_HAT_SWITCH     = 0x39,
  HID_USAGE_DESKTOP_COUNTED_BUFFER = 0x3a,
  HID_USAGE_DESKTOP_BYTE_COUNT     = 0x3b,
  HID_USAGE_DESKTOP_MOTION_WAKEUP  = 0x3c,
  HID_USAGE_DESKTOP_START          = 0x3d,
  HID_USAGE_DESKTOP_SELECT         = 0x3e,
  //skip 0x3f (63)
  HID_USAGE_DESKTOP_VX             = 0x40,
  HID_USAGE_DESKTOP_VY             = 0x41,
  HID_USAGE_DESKTOP_VZ             = 0x42,
  HID_USAGE_DESKTOP_VBRX           = 0x43,
  HID_USAGE_DESKTOP_VBRY           = 0x44,
  HID_USAGE_DESKTOP_VBRZ           = 0x45,
  HID_USAGE_DESKTOP_VNO            = 0x46,

  HID_USAGE_DESKTOP_FEATURE_NOTIFICATION  = 0x47,

  HID_USAGE_DESKTOP_RESOLUTION_MULTIPLIER = 0x48,

  HID_USAGE_DESKTOP_SYSTEM_CONTROL        = 0x80,
  HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN     = 0x81,
  HID_USAGE_DESKTOP_SYSTEM_SLEEP          = 0x82,
  HID_USAGE_DESKTOP_SYSTEM_WAKE_UP        = 0x83,
  HID_USAGE_DESKTOP_SYSTEM_CONTEXT_MENU   = 0x84,
  HID_USAGE_DESKTOP_SYSTEM_MAIN_MENU      = 0x85,
  HID_USAGE_DESKTOP_SYSTEM_APP_MENU       = 0x86,
  HID_USAGE_DESKTOP_SYSTEM_MENU_HELP      = 0x87,
  HID_USAGE_DESKTOP_SYSTEM_MENU_EXIT      = 0x88,
  HID_USAGE_DESKTOP_SYSTEM_MENU_SELECT    = 0x89,
  HID_USAGE_DESKTOP_SYSTEM_MENU_RIGHT     = 0x8a,
  HID_USAGE_DESKTOP_SYSTEM_MENU_LEFT      = 0x8b,
  HID_USAGE_DESKTOP_SYSTEM_MENU_UP        = 0x8c,
  HID_USAGE_DESKTOP_SYSTEM_MENU_DOWN      = 0x8d,
  HID_USAGE_DESKTOP_SYSTEM_COLD_RESTART   = 0x8e,
  HID_USAGE_DESKTOP_SYSTEM_WARM_RESTART   = 0x8f,

  HID_USAGE_DESKTOP_DPAD_UP    = 0x90,
  HID_USAGE_DESKTOP_DPAD_DOWN  = 0x91,
  HID_USAGE_DESKTOP_DPAD_RIGHT = 0x92,
  HID_USAGE_DESKTOP_DPAD_LEFT  = 0x93,

  HID_USAGE_DESKTOP_SYSTEM_DOCK           = 0xa0,
  HID_USAGE_DESKTOP_SYSTEM_UNDOCK         = 0xa1,
  HID_USAGE_DESKTOP_SYSTEM_SETUP          = 0xa2,
  HID_USAGE_DESKTOP_SYSTEM_BREAK          = 0xa3,
  HID_USAGE_DESKTOP_SYSTEM_DEBUGGER_BREAK = 0xa4,

  HID_USAGE_DESKTOP_APPLICATION_BREAK          = 0xa5,
  HID_USAGE_DESKTOP_APPLICATION_DEBUGGER_BREAK = 0xa6,

  HID_USAGE_DESKTOP_SYSTEM_SPEAKER_MUTE                   = 0xa7,
  HID_USAGE_DESKTOP_SYSTEM_HIBERNATE                      = 0xa8,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_INVERT                 = 0xb0,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_INTERNAL               = 0xb1,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_EXTERNAL               = 0xb2,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_BOTH                   = 0xb3,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_DUAL                   = 0xb4,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_TOGGLE_INT_EXT         = 0xb5,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_SWAP_PRIMARY_SECONDARY = 0xb6,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_LCD_AUTOSCALE          = 0xb7
};

// HID desktop pages
enum {
  HID_USAGE_DESKTOP_UNDEFINED             = 0x00,
  HID_USAGE_DESKTOP_POINTER               = 0x01,
  HID_USAGE_DESKTOP_MOUSE                 = 0x02,
  HID_USAGE_DESKTOP_JOYSTICK              = 0x04,
  HID_USAGE_DESKTOP_GAMEPAD               = 0x05,
  HID_USAGE_DESKTOP_KEYBOARD              = 0x06,
  HID_USAGE_DESKTOP_KEYPAD                = 0x07,
  HID_USAGE_DESKTOP_MULTI_AXIS_CONTROLLER = 0x08,
  HID_USAGE_DESKTOP_TABLET_PC_SYSTEM      = 0x09,
};

#define HID_USAGE_PAGE(x)    0x5, x
#define HID_USAGE(x)         0x9, x
#define HID_COLLECTION(x)    0xa1, x
#define HID_COLLECTION_END   0xc0
#define HID_REPORT_COUNT(x)  0x95, x
#define HID_REPORT_SIZE(x)   0x75, x
#define HID_LOGICAL_MIN(x)   0x15, x
#define HID_LOGICAL_MAX(x)   0x26, (x & 0xff), (x >> 8)
#define HID_PHYSICAL_MIN(x)  0x35, x
#define HID_PHYSICAL_MAX(x)  0x46, (x & 0xff), (x >> 8)
#define HID_INPUT(x)         0x81, x
#define HID_PUSH             0xa4
#define HID_POP              0xb4
#define HID_USAGE_MIN(x)     0x19, x
#define HID_USAGE_MAX(x)     0x29, x
#define HID_REPORT_ID(x)     0x85, x

#define HID_DATA             (0<<0)
#define HID_CONSTANT         (1<<0) //1
#define HID_ARRAY            (0<<1)
#define HID_VARIABLE         (1<<1) //2
#define HID_ABSOLUTE         (0<<2)
#define HID_RELATIVE         (1<<2) //4
#define HID_WRAP_NO          (0<<3)
#define HID_WRAP             (1<<3) //8
#define HID_LINEAR           (0<<4)
#define HID_NONLINEAR        (1<<4) //16
#define HID_PREFERRED_STATE  (0<<5)
#define HID_PREFERRED_NO     (1<<5) //32
#define HID_NO_NULL_POSITION (0<<6)
#define HID_NULL_STATE       (1<<6) //64
#define HID_NON_VOLATILE     (0<<7)
#define HID_VOLATILE         (1<<7) //128
#define HID_BITFIELD         (0<<8)
#define HID_BUFFERED_BYTES   (1<<8) //256

#define ENCODE_1(x)          x
#define ENCODE_2(x)          (x & 0xff), (x >> 8)
#define ENCODE_3(x)          (x & 0xff), (x >> 24), (x >> 16), (x >> 8)

#define HID_REPORT_ITEM_N    (data, tag, type, size) (((tag) << 4) | ((type) << 2) | (size)), ENCODE_##size(data)
#define HID_REPORT_ITEM_8    (data, tag, type) (((tag) << 4) | ((type) << 2) | (1)), ENCODE_1(data)
#define HID_REPORT_ITEM_16   (data, tag, type) (((tag) << 4) | ((type) << 2) | (2)), ENCODE_2(data)
#define HID_REPORT_ITEM_32   (data, tag, type) (((tag) << 4) | ((type) << 2) | (3)), ENCODE_3(data)

typedef struct {
  int report_size;
  int report_count;
  int usage_page;
  int usage[256];
  int next_usage;
  int usage_min;
} hid_state;

hid_state stack[16];
uint8_t sp = 0;
uint8_t bits = 0;

void addbyte(uint8_t **p, uint8_t byte) {
  *((*p)++) = byte;
}

void addshort(uint8_t **p, uint16_t d) {
  *((*p)++) = d & 0xff;
  *((*p)++) = d >> 8;
}

void hid_usage_page(uint8_t **p, uint8_t page) {
  addbyte(p, 0x5);
  addbyte(p, page);
  stack[sp].usage_page = page;
}

void hid_usage(uint8_t **p, uint8_t usage) {
  addbyte(p, 0x9);
  addbyte(p, usage);
  stack[sp].usage[stack[sp].next_usage++] = usage;
}

void hid_collection(uint8_t **p, uint8_t collection) {
  addbyte(p, 0xa1);
  addbyte(p, collection);
}

void hid_collection_end(uint8_t **p) {
  addbyte(p, 0xc0);
}

int get_bytes(uint16_t d) {
  if ((d >= 0) && (d <= UINT8_MAX)) return 1;
  else if ((d >= 0) && (d <= UINT16_MAX)) return 2;
}

void add_variable(uint8_t **p, uint16_t d, int bytes) {
  switch (bytes) {
  case 1:
    addbyte(p, d);
    break;
  case 2:
    addshort(p, d);
    break;
  }
}

void hid_report_size(uint8_t **p, uint8_t size) {
  addbyte(p, 0x75);
  addbyte(p, size);
  stack[sp].report_size = size;
}

void hid_report_count(uint8_t **p, uint16_t count) {
  int bytes = get_bytes(count);
  addbyte(p, 0x94 | bytes);
  add_variable(p, count, bytes);
  stack[sp].report_count = count;
}

void hid_logical_min(uint8_t **p, uint8_t min) {
  addbyte(p, 0x15);
  addbyte(p, min);
}

void hid_logical_max(uint8_t **p, uint16_t max) {
  int bytes = get_bytes(max);

  addbyte(p, 0x24 | bytes);
  add_variable(p, max, bytes);
}

void hid_physical_min(uint8_t **p, uint16_t min) {
  int bytes = get_bytes(min);

  addbyte(p, 0x34 | bytes);
  add_variable(p, min, bytes);
}

void hid_physical_max(uint8_t **p, uint16_t max) {
  int bytes = get_bytes(max);

  addbyte(p, 0x44 | bytes);
  add_variable(p, max, bytes);
}

