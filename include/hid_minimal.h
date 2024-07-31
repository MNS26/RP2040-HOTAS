#pragma once
#include <assert.h>

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


/// HID Usage Table - Table 5: Simulation Controls Page
enum {
  HID_CONTROL_PAGE_UNDEFINED                      = 0x00,
  HID_CONTROL_PAGE_FLIGHT_SIMULATION              = 0x01,
  HID_CONTROL_PAGE_AUTOMOBILE_SIMULATION          = 0x02,
  HID_CONTROL_PAGE_TANK_SIMULATION                = 0x03,
  HID_CONTROL_PAGE_SPACESHIP_SIMULATION           = 0x04,
  HID_CONTROL_PAGE_SUBMARINE_SIMULATION           = 0x05,
  HID_CONTROL_PAGE_SAILING_SIMULATION             = 0x06,
  HID_CONTROL_PAGE_MOTORCYCLE_SIMULATION          = 0x07,
  HID_CONTROL_PAGE_SPORTS_SIMULATION              = 0x08,
  HID_CONTROL_PAGE_AIRPLANE_SIMULATION            = 0x09,
  HID_CONTROL_PAGE_MAGIC_CARPET_SIMULATION        = 0x0A,
  HID_CONTROL_PAGE_HELICOPTER_SIMULATION          = 0x0B,
  HID_CONTROL_PAGE_BICYCLE_SIMULATION             = 0x0C,
  // RESERVED 0D - 1F
  HID_CONTROL_PAGE_FLIGHT_CONTROL_STICK            = 0x20,
  HID_CONTROL_PAGE_FLIGHT_STICK                    = 0x21,
  HID_CONTROL_PAGE_CYCLIC_CONTROL                  = 0x22,
  HID_CONTROL_PAGE_CYCLIC_TRIM                     = 0x23,
  HID_CONTROL_PAGE_FLIGHT_YOKE                     = 0x24,
  HID_CONTROL_PAGE_TRACK_CONTROL                   = 0x25,
  // RESERVED 26 - AF
  HID_CONTROL_PAGE_AILERON                         = 0xB0,
  HID_CONTROL_PAGE_AILERON_TRIM                    = 0xB1,
  HID_CONTROL_PAGE_ANTI_TORQUE_CONTROL             = 0xB2,
  HID_CONTROL_PAGE_AUTOPILOT_ENABLE                = 0xB3,
  HID_CONTROL_PAGE_CHAFF_RELEASE                   = 0xB4,
  HID_CONTROL_PAGE_COLLECTIVE_CONTROL              = 0xB5,
  HID_CONTROL_PAGE_DRIVE_BRAKE                     = 0xB6,
  HID_CONTROL_PAGE_ELECTRIC_COUNTERMEASURES        = 0xB7,
  HID_CONTROL_PAGE_ELEVATOR                        = 0xB8,
  HID_CONTROL_PAGE_ELEVATOR_TRIM                   = 0xB9,
  HID_CONTROL_PAGE_RUDDER                          = 0xBA,
  HID_CONTROL_PAGE_THROTTLE                        = 0xBB,
  HID_CONTROL_PAGE_FLIGHT_COMMUNICATIONS           = 0xBC,
  HID_CONTROL_PAGE_FLARE_RELEASE                   = 0xBD,
  HID_CONTROL_PAGE_LANDING_GEAR                    = 0xBE,
  HID_CONTROL_PAGE_TOE_BRAKE                       = 0xBF,
  HID_CONTROL_PAGE_TRIGGER                         = 0xC0,
  HID_CONTROL_PAGE_WEAPONS_ARM                     = 0xC1,
  HID_CONTROL_PAGE_WEAPONS_SELECT                  = 0xC2,
  HID_CONTROL_PAGE_WING_FLAPS                      = 0xC3,
  HID_CONTROL_PAGE_ACCELERATOR                     = 0xC4,
  HID_CONTROL_PAGE_BRAKE                           = 0xC5,
  HID_CONTROL_PAGE_CLUTCH                          = 0xC6,
  HID_CONTROL_PAGE_SHIFTER                         = 0xC7,
  HID_CONTROL_PAGE_STEERING                        = 0xC8,
  HID_CONTROL_PAGE_TURRET_DIRECTION                = 0xC9,
  HID_CONTROL_PAGE_BARREL_ELEVATION                = 0xCA,
  HID_CONTROL_PAGE_DIVE_PLANE                      = 0xCB,
  HID_CONTROL_PAGE_BALLAST                         = 0xCC,
  HID_CONTROL_PAGE_BICYCLE_CRANK                   = 0xCD,
  HID_CONTROL_PAGE_HANDLE_BARS                     = 0xCE,
  HID_CONTROL_PAGE_FRONT_BRAKE                     = 0xCF,
  HID_CONTROL_PAGE_REAR_BRAKE                      = 0xD0,
  // RESERVED D1 - FFFF
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
  int reportid;
} hid_state;

hid_state stack[16];
uint8_t sp = 0;
uint8_t bits = 0;

#ifdef DESKTOP_ONLY
typedef struct {
  int offset;
  int bits;
  uint32_t usage_page;
  uint32_t usage;
  uint input_variable;
} output;

#define MAX_REPORT_ID 31
#define MAX_OUTPUTS 256
output outputs[MAX_REPORT_ID][MAX_OUTPUTS];
unsigned int output_count[MAX_REPORT_ID];
unsigned int output_index = 0;
#endif

void addbyte(uint8_t **p, uint8_t _byte) {
  *((*p)++) = _byte;
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

void hid_report_id(uint8_t **p, uint8_t id) {
  addbyte(p, 0x85);
  addbyte(p, id);
  stack[sp].reportid = id;
  assert(id < MAX_REPORT_ID+1);
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
  else return 3;
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
#
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

void hid_input(uint8_t **p, uint8_t input) {
  addbyte(p, 0x81);
  addbyte(p, input);

  for (int i=stack[sp].next_usage - stack[sp].report_count; i<stack[sp].next_usage; i++) {
#ifdef DESKTOP_ONLY
    int usage;
    if (i >= 0) usage = stack[sp].usage[i];
    else usage = 0;


    assert(output_index < MAX_OUTPUTS+1);
    output *o = &outputs[stack[sp].reportid][output_index];
    o->offset = bits;
    o->bits = stack[sp].report_size;
    o->usage_page = stack[sp].usage_page;
    o->usage = usage;
    o->input_variable = input;

    output_index++;
    output_count[stack[sp].reportid] = output_index;
#endif
    bits += stack[sp].report_size;
  }
  stack[sp].next_usage = 0;
}

void hid_push(uint8_t **p) {
  addbyte(p, 0xa4);
}

void hid_pop(uint8_t **p) {
  addbyte(p, 0xb4);
}

void hid_usage_min(uint8_t **p, uint8_t min) {
  addbyte(p, 0x19);
  addbyte(p, min);
  stack[sp].usage_min = min;
}

void hid_usage_max(uint8_t **p, uint8_t max) {
  addbyte(p, 0x29);
  addbyte(p, max);
  for (int n=stack[sp].usage_min; n<= max; n++) {
    stack[sp].usage[stack[sp].next_usage++] = n;
  }
}
