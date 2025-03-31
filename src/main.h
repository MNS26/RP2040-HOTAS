

//sd_card_t *pSD;
FATFS fs;
FRESULT fr;
FIL fil;
//IniConfig ini;
//YAML yaml;
std::string str_config;
//=======================


//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
  BLINK_ERROR = 100,
};

uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

typedef struct {
  uint8_t buttons;
  uint8_t hats;
  uint8_t axis;

} input_id;


typedef union TU_ATTR_PACKED{
  struct {
    uint8_t type:2;
    uint8_t id:6;
  };
  uint8_t option;
}input_type;
input_id inputs_id[MAX_REPORT_ID];
uint8_t report = 1;

typedef struct {
  std::string DeviceName;
  bool EnableKeyboard = false;
  bool EnableMouse = false;
  bool EnableSerial = false;
  uint16_t AxisResolution = 11;
  uint16_t ADCResolution = 11;
  uint16_t AxisCount = 8;
  uint16_t ButtonCount = 60;
  uint8_t HatCount = 1;
//  uint8_t HidUsagePageVal = HID_USAGE_PAGE_DESKTOP;
//  uint8_t HidUsageVal = HID_USAGE_DESKTOP_JOYSTICK;
  /* data */
} Config;


Config config;

bool enableKeyboard = 0;
bool enableMouse = 0;
bool enableCdc = 1;
bool enableMsc = 1;
bool enableMtp = 0;
uint8_t IdCount;
uint8_t MaxDeviceCount = MAX_REPORT_ID; 

uint16_t usb_hid_descriptor_length;
uint8_t usb_hid_descriptor[MAX_HID_DESCRIPTOR_SIZE];

uint16_t axis_start[MAX_REPORT_ID];
uint16_t button_start[MAX_REPORT_ID];
uint16_t hat_start[MAX_REPORT_ID];
uint16_t total_bits[MAX_REPORT_ID];

volatile uint8_t reports[MAX_REPORT_ID][64];
uint8_t old_reports[MAX_REPORT_ID][64];
volatile bool readyToUpdate[MAX_REPORT_ID];

// linux only sees up to 79 buttons
uint8_t device_max_button_count = 79;
// to keep linux support its set to 1, windows can go up to 4
uint8_t device_max_hat_count = 1;
// linux actually sees 9 but wine (windows compatibility layer) only works up to 8
uint8_t device_max_axis_count = 8;
std::string DeviceName;
uint8_t AxisResolution = 11;
uint8_t ADCResolutionCurrent;
uint8_t ADCResolution = 11;
uint16_t AxisCount = 8;
uint16_t ButtonCount = 60;
uint16_t HatCount = 1;
//uint8_t hid_usage_page_val = HID_USAGE_PAGE_DESKTOP;
//uint8_t hid_usage_val = HID_USAGE_DESKTOP_JOYSTICK;

//USBD_CDC Serial;
//USBD_HID usb_hid;
//USBD_MSC usb_msc;
//USBD_MTP usb_mtp;
uint initial = 1000;
uint last_millis = 0;

void led_blinking_task(void);
//int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize);
//int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize);
//void msc_flush_cb(void);
void cdc_task(void);
void mount_sdcard(void);
void get_config(const char *path);
void setup_hid_descriptor(void);
//void setup_usb_configurator(void);
//uint16_t get_report_callback (uint8_t , hid_report_type_t , uint8_t* , uint16_t);
//void set_report_callback(uint8_t , hid_report_type_t , uint8_t const* , uint16_t );
void set_button(volatile uint8_t *report, int reportid, uint button, uint value);
void set_axis(volatile uint8_t *report, int reportid, int axis, uint32_t value);
void set_hat(volatile uint8_t *report, uint8_t reportid, uint8_t hat, uint8_t value);
template <typename T>
T map_clamped(T x, T in_min, T in_max, T out_min, T out_max) ;
template <typename T>
T deadzone(T x, T midpoint, uint8_t deadzoneSize);

