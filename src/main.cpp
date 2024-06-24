/*== JOYSTICK ==*/


#define MAX_USB_PACKET_SIZE 64
#define MAX_HID_DESCRIPTOR_SIZE 4096

// thechnically there is no ID cap since its limited to the descriptor size but... for ease of use we set it
#define MAX_REPORT_ID 31 
#define MAX_OUTPUTS 256

#include <Arduino.h>

#include "Commands.h"
#include "Common.h"
#include "hid_minimal.h"
#include "common/tusb_common.h"
#include "hid.h"
#include "HID_descriptor.h"
#include "HID_Report.h"
#include <Adafruit_TinyUSB.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "CRC.h"
#include "Wire.h"




bool LED_on;
bool LED_breathe;
uint8_t LED_brightness;
int speed=1;
int LED=0;
uint32_t lastTime = millis();
int report = 1;

/*== LED ==*/
Adafruit_NeoPixel pixel;

bool enableMouseAndKeyboard = 0;
uint8_t DeviceCount;

uint16_t hue;
uint32_t nextScan = millis();
uint32_t nextScanPrint = millis();
uint usb_report_size;
uint8_t usb_report[MAX_HID_DESCRIPTOR_SIZE];

typedef struct {
  uint8_t buttons;
  uint8_t hats;
  uint8_t axis;

} input_id;
input_id inputs_id[MAX_REPORT_ID]; 

uint16_t axis_start[MAX_REPORT_ID];
uint16_t button_start[MAX_REPORT_ID];
uint16_t hat_start[MAX_REPORT_ID];
uint16_t total_bits[MAX_REPORT_ID];
uint16_t largest_bits;

volatile uint8_t reports[MAX_REPORT_ID][64];
uint8_t old_reports[MAX_REPORT_ID][64];
volatile bool readyToUpdate[MAX_REPORT_ID];

// linux only sees up to 79 buttons
uint8_t device_max_button_count = 79;
// to keep linux support its set to 1, windows can go up to 4
uint8_t device_max_hat_count = 1;
// linux actually sees 9 but wine (windows compatibility layer) only works up to 8
uint8_t device_max_axis_count = 8;

uint32_t AxisResolution = 11;
uint16_t AxisCount = 8;
uint16_t ButtonCount = 65;
uint16_t HatCount = 1;

uint8_t hid_usage_page_val = HID_USAGE_PAGE_DESKTOP;
uint8_t hid_usage_val = HID_USAGE_DESKTOP_JOYSTICK;

Adafruit_USBD_HID hid_joystick;
Adafruit_USBD_HID hid_kbm;

byte i2cIDs[128];

// Select the FileSystem by uncommenting one of the lines below

//#define USE_SPIFFS
//#define USE_LITTLEFS
#define USE_SDFS

#if defined USE_SPIFFS
#include <FS.h>
const char* fsName = "SPIFFS";
FS* fileSystem = &SPIFFS;
SPIFFSConfig fileSystemConfig = SPIFFSConfig();
#elif defined USE_LITTLEFS
#include <LittleFS.h>
const char* fsName = "LittleFS";
FS* fileSystem = &LittleFS;
LittleFSConfig fileSystemConfig = LittleFSConfig();
#elif defined USE_SDFS
#include <SDFS.h>
const char* fsName = "SDFS";
FS* fileSystem = &SDFS;
SDFSConfig fileSystemConfig = SDFSConfig();
// fileSystemConfig.setCSPin(chipSelectPin);
#else
#error Please select a filesystem first by uncommenting one of the "#define USE_xxx" lines at the beginning of the sketch.
#endif
void setupUSB();

static bool fsOK;
#ifdef ARDUINO_RASPBERRY_PI_PICO_W //todo add more wifi boards (eg esp32)
#include "webInterface.h"
extern FS* fileSystem;
#endif


#undef bitSet
#define bitSet(value, bit) ((value) |= (1ULL << (bit)))

#undef bitClear
#define bitClear(value, bit) ((value) &= ~(1ULL << (bit)))


template <typename T>
T map_clamped(T x, T in_min, T in_max, T out_min, T out_max) {
  if (x < in_min) x = in_min;
  if (x > in_max) x = in_max;
  x = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return x;
}

template <typename T>
T deadzone(T x, T midpoint, uint8_t deadzoneSize) {
  if (abs(x - midpoint) < deadzoneSize) {
    return midpoint;
  }
  return x;
}

uint32_t rainbow(uint16_t _hue, uint8_t saturation, uint8_t brightness, bool gammify) {
  uint32_t color = pixel.ColorHSV(_hue, saturation, brightness);
  if (gammify) color = pixel.gamma32(color);
  return color;
}


// this is here since the joystick still works and doesnt need to be replaced (it will be gutted after throttle is properly done)

#define X52_BUSY_WAIT 0
#define X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION 1
#include <x52_hotas.h>
#define MAX_UPDATES_PER_SECOND 300

// TODO: Choose your favorite digital pins on your board.
x52::pro::JoystickClient<D22, D21, D26, D20> joystick_client;
//x52::pro::JoystickConfig cfg;
//x52::pro::JoystickState state;

/*
  =============
    Callbacks  
  =============
*/

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  Serial.print("get_report type: ");
  Serial.println((int)report_type);
  Serial.print("get_report id: ");
  Serial.println((int)report_id);
  switch (report_type)
  {
    case (hid_report_type_t)HID_REPORT_TYPE_INVALID:
    case (hid_report_type_t)HID_REPORT_TYPE_OUTPUT:
    break;

    case (hid_report_type_t)HID_REPORT_TYPE_INPUT:
    case (hid_report_type_t)HID_REPORT_TYPE_FEATURE:	
    return reqlen;
  }
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  Serial.print("set_report type: ");
  Serial.println((int)report_type);
  Serial.print("set_report id: ");
  Serial.println(report_id);
 
  switch (report_type)
  {
    case HID_REPORT_TYPE_INVALID:
    case HID_REPORT_TYPE_INPUT:
    case HID_REPORT_TYPE_OUTPUT:
    break;

    case HID_REPORT_TYPE_FEATURE:
    break;
  }
}


bool getButtonI2cSlave (int slaveID, int inputID) {
  Wire.beginTransmission(slaveID);
  Wire.write(GetButton);
  Wire.write(inputID);
  Wire.endTransmission();
  uint8_t buffsize = sizeof(bool);
  Wire.requestFrom(slaveID, buffsize);
  return Wire.read();
}

template <typename T> T getAxisI2cSlave (int slaveID, int inputID) {
  Wire.beginTransmission(slaveID);
  Wire.write(GetAxis);
  Wire.write(inputID);
  Wire.endTransmission();
  Wire.requestFrom(slaveID, sizeof(T));
  T buf;
  for (int i = 0; Wire.available()>0; i++) {
    ((uint8_t*)buf)[i] = Wire.read();
  }
  return (T)buf;
}

uint32_t getLedI2cSlave (int slaveID, int LedID) {
  Wire.beginTransmission(slaveID);
  Wire.write(GetLed);
  Wire.write(LedID);
  Wire.endTransmission();
  Wire.requestFrom(slaveID, sizeof(uint32_t));
  uint32_t buf = 0;
  for (int i = 0; Wire.available()>0; i++) {
    ((uint8_t*)buf)[i] = Wire.read();
  }
  return buf;
}

void setLedI2cSlave (int slaveID, int LedID, uint32_t rgbw) {
  Wire.beginTransmission(slaveID);
  Wire.write(GetLed);
  Wire.write(LedID);
  Wire.write(rgbw);
  Wire.endTransmission();
}

// I2C scanner
void SerialI2cScanner() {
  Serial.print("\n\r    I2C Bus Scan\n\r");
  Serial.print("    0 1 2 3 4 5 6 7 8 9 A B C D E F\n\r");
  for (int addr = 0; addr < (1 << 7); ++addr) {
    if (addr % 16 == 0) {
      Serial.printf("%02x: ", addr);
    }
    if (i2cIDs[addr])
      Serial.print("X");
    else if (addr < 0x80)
      Serial.print("-");
    else
      Serial.print(" ");
    Serial.print(addr % 16 == 15 ? "\n\r" : " ");
  }
}


void set_button(volatile uint8_t *report, int reportid, uint button, uint value) {
  if (inputs_id[reportid].buttons == 0 || inputs_id[reportid].buttons<button)
    return;
  int bit = button_start[reportid] + button;
  int byte = bit / 8;
  int shift = bit % 8;

  report[byte] |= (value & 1) << shift;
}

void set_axis(volatile uint8_t *report, int reportid, int axis, uint32_t value) {
  if (inputs_id[reportid].axis == 0 || inputs_id[reportid].axis<axis)
    return;
  int remaining_bits = AxisResolution;
  int bitoffset = axis_start[reportid]+(AxisResolution*axis);
  while (remaining_bits) {
    int byteoffset = bitoffset / 8;
    int shift = bitoffset % 8;
    int bits_that_can_fit = MIN(8 - shift, remaining_bits);
    uint32_t lower_bits = ((1 << bits_that_can_fit)-1) & value;
    report[byteoffset] |= lower_bits << shift;
    value = value >> bits_that_can_fit;
    remaining_bits -= bits_that_can_fit;
    bitoffset += bits_that_can_fit;
  }
}

void set_hat(volatile uint8_t *report, uint8_t reportid, uint8_t hat, uint8_t value) {
  if (inputs_id[reportid].hats == 0 || inputs_id[reportid].hats<hat)
    return;
  uint8_t remaining_bits = 4; // resolution of hats
  int bitoffset = hat_start[reportid] + (hat * remaining_bits);
  while (remaining_bits) {
    int byteoffset = bitoffset / 8;
    int shift = bitoffset % 8;
    int bits_that_can_fit = MIN(8 - shift, remaining_bits);
    uint32_t lower_bits = ((1 << bits_that_can_fit)-1) & value;
    report[byteoffset] |= lower_bits << shift;
    value = value >> bits_that_can_fit;
    remaining_bits -= bits_that_can_fit;
    bitoffset += bits_that_can_fit;
  }
}

void progressCallBack(size_t currSize, size_t totalSize) {
	  Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
}


void setupUSB() {
  // reset the connection
  memset(inputs_id, 0, sizeof(inputs_id));
  memset(usb_report, 0, sizeof(usb_report));
  memset(button_start,0,sizeof(button_start));
  memset(hat_start,0,sizeof(hat_start));
  memset(axis_start,0,sizeof(axis_start));
  memset(total_bits,0,sizeof(total_bits));
  usb_report_size=0;
  largest_bits=0;
  DeviceCount=0;

  /* 
    Calculate how many "devices" we will need to emulate
    Calcucate how many buttons per "device"
    Calculate how many hats per "device"
    Calculate how many axis per "device"
  */
 
  for (uint16_t i=ButtonCount, j=1; i>0; i-=MIN(device_max_button_count,i), j++) {
    inputs_id[j].buttons = i>device_max_button_count ? device_max_button_count:i;
    DeviceCount = j>DeviceCount ? j:DeviceCount;
  }
  for (uint16_t i=HatCount, j=1; i>0; i-=MIN(device_max_hat_count, i), j++) {   
    inputs_id[j].hats = i>device_max_hat_count ? device_max_hat_count:i;
    DeviceCount = j>DeviceCount ? j:DeviceCount;
  }
  for (uint8_t i=AxisCount, j=1; i>0;i-=MIN(device_max_axis_count,i), j++) {
    inputs_id[j].axis = i>device_max_axis_count ? device_max_axis_count:i;
    DeviceCount = j>DeviceCount ? j:DeviceCount;
  }
  for (uint16_t i = 1; i<=DeviceCount; i++) {
    makeDescriptor(i, AxisResolution, inputs_id[i].axis, inputs_id[i].hats, inputs_id[i].buttons, usb_report, &usb_report_size);
  }
  for (uint8_t rep = 1; rep < MAX_REPORT_ID; rep++) {
    largest_bits = (largest_bits < total_bits[rep]) ? total_bits[rep] : largest_bits;
  }

  Serial.printf("total bytes %d", usb_report_size);
  Serial.print("EEEEEE");
  TinyUSBDevice.detach();

  TinyUSBDevice.setID(VID,PID);
  TinyUSBDevice.setManufacturerDescriptor("Raspberry Pi");
  TinyUSBDevice.setProductDescriptor("RP2040-HOTAS");

  // start the USB device
  USBDevice.attach();
  SerialTinyUSB.begin(115200);
  
  if (DeviceCount > 0) {
    hid_joystick.setPollInterval(1);
    hid_joystick.setBootProtocol(HID_ITF_PROTOCOL_NONE);
    hid_joystick.setReportDescriptor(usb_report, usb_report_size);
    hid_joystick.setReportCallback(get_report_callback, set_report_callback);
    hid_joystick.begin();
  }
  if (enableMouseAndKeyboard) {
    // HID report descriptor using TinyUSB's template
    uint8_t desc_hid_report[] = {
      TUD_HID_REPORT_DESC_KEYBOARD( VA_HID_REPORT_ID(1) ),
      TUD_HID_REPORT_DESC_MOUSE   ( VA_HID_REPORT_ID(2) ),
    };

    hid_kbm.setStringDescriptor("TinyUSB Keyboard/Mouse");
    hid_kbm.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    hid_kbm.begin();
  }
}
/*
  =========
    Setup
  =========
 */

void setup() {

#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as
  // - mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  setupUSB();



  //SPI.setRX(4);
  //SPI.setTX(7);
  //SPI.setSCK(6);
  //SPI.setCS(5);
  fileSystemConfig.setCSPin(17);

  ////////////////////////////////
  // FILESYSTEM INIT

  fileSystemConfig.setAutoFormat(false);
  fileSystem->setConfig(fileSystemConfig);
  fsOK = fileSystem->begin();
  //DBG_OUTPUT_PORT.println(fsOK ? F("Filesystem initialized.") : F("Filesystem init failed!"));

#ifdef ARDUINO_RASPBERRY_PI_PICO_W
  setupWifi();
  configTime(3 * 3600, 1, "nl.pool.ntp.org", "0.europe.pool.ntp.org");
#endif

}




/*
  ========
    Loop
  ========
 */
void loop()
{
#ifdef ARDUINO_RASPBERRY_PI_PICO_W
  server.handleClient();
  MDNS.update();

  // if (millis() - check_firmware_update > 1000) {
  //   if (fileSystem->exists("/firmware.bin")) {
  //     File firmware =  fileSystem->open("/firmware.bin","r");
  //     if (firmware) {
  //       Serial.println(F("found!"));
  //       Serial.println(F("Try to update!"));
  //       Update.onProgress(progressCallBack);
  //       Update.begin(firmware.size(), U_FLASH);
  //       Update.writeStream(firmware);
  //       if (Update.end()){
  //         Serial.println(F("Update finished!"));
  //         rebootUpdate = true;
  //       }else{
  //         Serial.println(F("Update error!"));
  //         Serial.println(Update.getError());
  //       }
  //       firmware.close();
  //       if (fileSystem->rename("/firmware.bin", "/firmware.bak")){
  //         Serial.println(F("Firmware rename succesfully!"));
  //       }else{
  //         Serial.println(F("Firmware rename error!"));
  //       }
  //     }
  //     if (rebootUpdate) {
  //       rp2040.reboot();
  //     }
  //   }
  // }
#endif

  if(TinyUSBDevice.ready()) {
    if (millis() - lastTime > 100) {
      lastTime = millis();
      LED += speed;
      if (LED > 31 || LED < 0)
        speed = -speed;
      if (LED > 31)
        LED = 31;
      if (LED < 0)
        LED = 0;
    }
  } else if (TinyUSBDevice.suspended())
  {
    LED = 0;
  }	else {
    LED = 1;
  }

  // Wake up host if we are in suspend mode
  // and REMOTE_WAKEUP feature is enabled by host
  //if (TinyUSBDevice.suspended() && BOOTSEL)
  //{
  //	TinyUSBDevice.remoteWakeup();
  //}
  

  //if (rp2040.fifo.available()) {
  //  uint32_t t;
  //  rp2040.fifo.pop_nb(t);
  //}
  if (TinyUSBDevice.ready()) {
    if (readyToUpdate[report] ){
        hid_joystick.sendReport(report, (void *)reports[report], total_bits[report]/8);
        memcpy(old_reports[report],(void *)reports[report],total_bits[report]/8);
        readyToUpdate[report] = false;
    }
    report++;
    if (report >= DeviceCount)
      report = 1;
  }
}




void setup1()
{

#if defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
  pinMode(23,OUTPUT);
  digitalWrite(23,HIGH); // this forces the switch mode power supply into pwm mode (less efficient but also less noise) 
#endif
  pinMode(27,INPUT);
  pinMode(28,INPUT);
  pinMode(0, OUTPUT_2MA);
  pinMode(1, OUTPUT_2MA);
  pinMode(15, OUTPUT_2MA);
  pinMode(14, OUTPUT_2MA);
  pinMode(13, OUTPUT_2MA);
  pinMode(12, OUTPUT_2MA);
  pinMode(11, OUTPUT_2MA);
  pinMode(10, OUTPUT_2MA);
  pinMode(9, OUTPUT_2MA);
  pinMode(8, OUTPUT_2MA);
  digitalWrite(0, HIGH);
  digitalWrite(1, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  
  Wire.setTimeout(10);
  Wire.setSDA(0);
  Wire.setSCL(1);
#ifdef I2CSPEED
  Wire.setClock(I2CSPEED);
#else
  Wire.setClock(1000);
#endif
  Wire.setTimeout(100);
  Wire.begin();

  analogReadResolution(11);
  //setLedI2cSlave(0,0x21,0);
  //setLedI2cSlave(1,0x21,0);
  //setLedI2cSlave(2,0x21,0);
  
  joystick_client.Setup();

#if MAX_UPDATES_PER_SECOND
  static x52::util::RateLimiter<MAX_UPDATES_PER_SECOND, MAX_UPDATES_PER_SECOND> rate_limiter;
#endif

}


void loop1()
{

  if (millis() - nextScan > 100) {
      for (int addr = 8; addr < (1 << 7); ++addr) {

        // Perform a 0-byte read from the probe address. The read function
        // returns a negative result NAK'd any time other than the last data
        // byte. Skip over reserved addresses.
        int result;
        if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78)
          result = -1;
        else
          Wire.beginTransmission(addr);
        result = Wire.endTransmission();
        if (result == 0)
          i2cIDs[addr] = 1;
        else
          i2cIDs[addr] = 0;
      }
      nextScan = millis();
  }
  // if (millis() - nextScanPrint > 10000) {
  //   SerialI2cScanner();
  //   nextScanPrint = millis();
  // }



  // hue+=50;
  // if (hue >= UINT16_MAX)
  // hue = 0;

  // for (int p = 0; p < 3; p++) {
  //   uint16_t _hue = hue + (p * 1 * 65536) / 3;
  //   uint32_t _pixel = rainbow(_hue, 255, 255*map_clamped<float>(LED,0,31,0,1), true);
  //   auto buffsize = sizeof(command->command_type+command->id);
  //   buffsize += sizeof(_pixel);
  //   command->command_type = SetLed;
  //   command->id = p;
  //   command->data = (uint32_t*)_pixel;
  //   Wire.beginTransmission(0x21);
  //   Wire.write((byte *)i2cBuff, buffsize);
  //   Wire.endTransmission();
  //   memset(i2cBuff, 0 ,i2cBuffSize);

  // }


  // Calling PrepareForPoll is optional (this firmware works without it)
  // but it eliminates almost all jitter from the timing of the updates.
  // With PrepareForPoll we ask the joystick to prepare for the next
  // update so it can serve our PollJoystickState call immediately
  // after the delay introduced by the rate limiter.
  // Without PrepareForPoll it can take up to 1500ms for the joystick
  // to respond to our PollJoystickState call.
  joystick_client.PrepareForPoll();



#if MAX_UPDATES_PER_SECOND
  static x52::util::RateLimiter<MAX_UPDATES_PER_SECOND, MAX_UPDATES_PER_SECOND> rate_limiter;
  unsigned long d = rate_limiter.MicrosTillNextUpdate();
  if (d == 0) {
#endif
  // Query the state of the X52 Pro joystick via the PS/2 connection and send it to
  // the PC as the state of the USB joystick emulated by the Arduino-compatible board.
  x52::pro::JoystickConfig cfg;
  x52::pro::JoystickState state;
  cfg.led_brightness = LED;


  auto timeout_micros = joystick_client.PollJoystickState(state, cfg);
  if (timeout_micros) {
    //X52DebugPrintln("PollJoystickState failed");
    //delayMicroseconds(timeout_micros);
    //return;
  }
  bool empty = false;


  bool p2u = bool(state.pov_2 & x52::Up);
  bool p2d = bool(state.pov_2 & x52::Down);
  bool p2l = bool(state.pov_2 & x52::Left);
  bool p2r = bool(state.pov_2 & x52::Right);
  
  bool M1 = 0;
  bool M2 = 0; 
  bool M3 = 0;

switch (state.mode) {
    case x52::ModeUndefined: 
      M1 = 0;
      M2 = 0; 
      M3 = 0;
    break;
    case x52::Mode1: 
      M1 = 1;
      M2 = 0;
      M3 = 0;
    break;
    case x52::Mode2:
      M1 = 0;
      M2 = 1;
      M3 = 0;
    break;
    case x52::Mode3:
      M1 = 0;
      M2 = 0;
      M3 = 1;
    break;
  }

  // 1 = ??? (nub X?)
  // 2 = ??? (nub Y?)
  // 3 = I dial
  // 4 = E dial
  // 5 = ??? (slider ?)
  //Serial.print(jr[0].rx, BIN);
  //Serial.print(" ");
  //Serial.println(getAxisI2cSlave<uint16_t>(0x21, 5),BIN);

if (readyToUpdate[1]== false) {
    memset((void *)reports[1], 0, largest_bits/8);
    switch (state.pov_1) {
      case 0b0001:  set_hat(reports[1], 1, 0 ,0b0011); break;
      case 0b0011:  set_hat(reports[1], 1, 0 ,0b0100); break;
      case 0b0010:  set_hat(reports[1], 1, 0 ,0b0101); break;
      case 0b0110:  set_hat(reports[1], 1, 0 ,0b0110); break;
      case 0b0100:  set_hat(reports[1], 1, 0 ,0b0111); break;
      case 0b1100:  set_hat(reports[1], 1, 0 ,0b1000); break;
      case 0b1000:  set_hat(reports[1], 1, 0 ,0b0001); break;
      case 0b1001:  set_hat(reports[1], 1, 0 ,0b0010); break;
      default: set_hat(reports[1], 1, 0, 0b0000); break;
    }
  
    set_axis(reports[1], 1, 0, map_clamped<uint16_t>(deadzone<uint16_t>(state.x,512, 0), 0, 1023, 0, 2047));
    set_axis(reports[1], 1, 1, map_clamped<uint16_t>(deadzone<uint16_t>(state.y,512, 0), 0, 1023, 0, 2047));
    set_axis(reports[1], 1, 2, map_clamped<uint16_t>(deadzone<uint16_t>(state.z,512, 50), 0, 1023, 0, 2047));  //yaw
    set_axis(reports[1], 1, 3, map_clamped<uint16_t>(analogRead(A1), 300, 1700, 2047, 0));

    set_button(reports[1], 1, 0, state.trigger_stage_1);
    set_button(reports[1], 1, 1, state.button_fire);
    set_button(reports[1], 1, 2, state.button_a);
    set_button(reports[1], 1, 3, state.button_b);
    set_button(reports[1], 1, 4, state.button_c);
    set_button(reports[1], 1, 5, state.pinkie_switch);
    set_button(reports[1], 1, 6, getButtonI2cSlave(0x21, Button1));
    set_button(reports[1], 1, 7, getButtonI2cSlave(0x21, Button3));
    set_button(reports[1], 1, 8, state.button_t1);
    set_button(reports[1], 1, 9, state.button_t2);
    set_button(reports[1], 1, 10, state.button_t3);
    set_button(reports[1], 1, 11, state.button_t4);
    set_button(reports[1], 1, 12, state.button_t5);
    set_button(reports[1], 1, 13, state.button_t6);
    set_button(reports[1], 1, 14, state.trigger_stage_2);
    set_button(reports[1], 1, 15, getButtonI2cSlave(0x21, Button10));
    set_button(reports[1], 1, 16, empty); // scroll up
    set_button(reports[1], 1, 17, empty); //scroll down
    set_button(reports[1], 1, 18, getButtonI2cSlave(0x21, Button11));
    set_button(reports[1], 1, 19, p2u);
    set_button(reports[1], 1, 20, p2r);
    set_button(reports[1], 1, 21, p2d);
    set_button(reports[1], 1, 22, p2l);
    set_button(reports[1], 1, 23, getButtonI2cSlave(0x21, Button7));
    set_button(reports[1], 1, 24, getButtonI2cSlave(0x21, Button6));
    set_button(reports[1], 1, 25, getButtonI2cSlave(0x21, Button5));
    set_button(reports[1], 1, 26, getButtonI2cSlave(0x21, Button4));
    set_button(reports[1], 1, 27, M1);
    set_button(reports[1], 1, 28, M2);
    set_button(reports[1], 1, 29, M3);
    set_button(reports[1], 1, 30, getButtonI2cSlave(0x21, Button2));

  // 15/14 scroll mfd
  // 9/8 pg scroll


    set_button(reports[1], 1, 31, !digitalRead(10)); //function
    set_button(reports[1], 1, 32, !digitalRead(11)); //start/stop
    set_button(reports[1], 1, 33, !digitalRead(12)); //reset
    set_button(reports[1], 1, 38, !digitalRead(13)); // mfd select
    //bitWrite(jr[0].buttons, 34, !bitRead(jr[0].buttons,34)); //reset

    // if (!digitalRead(9)&& digitalRead(8)) { // pg up
    //   bitWrite(buttons, 34, 1);
    //   bitWrite(buttons, 35, 0);
    // } else if (!digitalRead(9)&& !digitalRead(8)) { //pg down
    //   bitWrite(buttons, 34, 0);
    //   bitWrite(buttons, 35, 1);
    // } else {
    //   bitWrite(buttons, 34, 0);
    //   bitWrite(buttons, 35, 1);
    // }


  //  if (!digitalRead(15)) {
  //    if (digitalRead(14)) {
  //      digitalWrite(LED_BUILTIN,1);
  //    } else {
  //      digitalWrite(LED_BUILTIN,0);
  //    }
  //  }
    // if (!digitalRead(15)&& digitalRead(14)) { // pg up
    //   bitWrite(buttons, 35, 1);
    //   bitWrite(buttons, 36, 0);
    // } else if (!digitalRead(15)&& !digitalRead(14)) { //pg down
    //   bitWrite(buttons, 35, 0);
    //   bitWrite(buttons, 36, 1);
    // } else {
    //   bitWrite(buttons, 35, 0);
    //   bitWrite(buttons, 36, 0);
    // }

    if (memcmp((void *)reports[1],old_reports[1], total_bits[1]/8) != 0) {
    //  Serial.println(memcmp((void *)reports[1],old_reports[1], total_bits[1]/8));
      readyToUpdate[1] = true;  
    }
  };
  //if (!digitalRead(10)||!digitalRead(11)||!digitalRead(12)||!digitalRead(13)) {digitalWrite(LED_BUILTIN,1);}else {digitalWrite(LED_BUILTIN,0);}  //if( rp2040.fifo.push_nb(buttons))
  //{
  //}  
  //jr[0].buttons = buttons;
  
#if MAX_UPDATES_PER_SECOND
  }
#endif
}
