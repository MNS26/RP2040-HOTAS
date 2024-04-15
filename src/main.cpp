/*== JOYSTICK ==*/
#include <Arduino.h>

#include "Commands.h"
#include "Common.h"
#include "hid.h"
#include "HID_descriptor.h"
#include "HID_Report.h"
#include <Adafruit_TinyUSB.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "CRC.h"
#include "Wire.h"


#define MAX_USB_PACKET_SIZE 64
#define MAX_HID_REPORTDESC_SIZE 4096



bool LED_on;
int speed=1;
int LED=0;
int lastTime = micros();
int update_cooldown = micros();
int report = 0;

/*== LED ==*/
Adafruit_NeoPixel pixel;

bool enableMouseAndKeyboard = 0;
uint8_t DeviceCountJoystick = 0;
uint8_t DeviceCountButtons = 0;
uint8_t DeviceCountHat = 0;
uint8_t DeviceCount = 0;
int COLLECTIONS = MAX_COLLECTIONS;

int AxisResolution = 11;
int AxisCount = 8;
int ButtonCount = 32;
int HatCount= 2;

hid_Joystick_report_t* jr = NULL;
hid_Joystick_report_t* jr_old = NULL;
uint8_t* _hidJoystickReportDesc = NULL;
uint16_t _hidJoystickReportDescSize = 0;
uint8_t* _hidKBMReportDesc = NULL;
uint16_t _hidKBMReportDescSize = 0;
size_t buffsize;
int updateButtons;
bool* readyToSend = NULL;

Adafruit_USBD_HID hid_joystick;
Adafruit_USBD_HID hid_kbm;

uint8_t i2cBuffSize = 255;
uint8_t i2cBuffUsed;

void* i2cBuff = NULL;
volatile Command* command;
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

static bool fsOK;
bool pauseForScan = false;
#ifdef ARDUINO_RASPBERRY_PI_PICO_W
#include "picow_only.h"
extern FS* fileSystem;
#endif




template <typename T>
T map_clamped(T x, T in_min, T in_max, T out_min, T out_max)
{
  if (x < in_min) x = in_min;
  if (x > in_max) x = in_max;
  if (x < out_min) x = out_max;
  if (x > out_max) x = out_max;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t rainbow(uint16_t _hue, uint8_t saturation, uint8_t brightness, bool gammify) {
  uint32_t color = pixel.ColorHSV(_hue, saturation, brightness);
  if (gammify) color = pixel.gamma32(color);
  return color;
}


/*
  =============
    Callbacks  
  =============
*/
#include "hid_Report_Callback.h"


bool getButtonI2cSlave (int inputID, int slaveID) {
  memset(i2cBuff, 0 ,i2cBuffSize);
  command->command_type = GetButton;
  command->id = inputID;
  buffsize = sizeof(command->command_type+command->id);
  Wire.beginTransmission(slaveID);
  Wire.write((uint8_t*)i2cBuff, buffsize);
  Wire.endTransmission();
  buffsize += sizeof(bool);
  memset(i2cBuff, 0 ,i2cBuffUsed);
  Wire.requestFrom(slaveID, buffsize);
  for (int i = 0; Wire.available()>0; i++) {
    ((uint8_t*)i2cBuff)[i] = Wire.read();
    i2cBuffUsed = i;
  }
  // Serial.print("COMMAND TYPE: ");
  // Serial.print(command->command_type);
  // Serial.print(" GetButton ID: ");
  // Serial.print(command->id);

  // Serial.print(" STATE: ");
  // Serial.println(*(uint8_t*)command->data);
  // Serial.print("BUFF: ");
  // for (int i = 0; i < i2cBuffUsed; i++) {
  //   Serial.print(((uint8_t*)i2cBuff)[i], BIN);
  // }
  // Serial.println();
  return *(bool *)command->data;
}
// IS CURRENTLY BROKEN!!!!!!
uint16_t getAxisI2cSlave (int inputID, int slaveID) {
  memset(i2cBuff, 0 ,i2cBuffUsed);
  command->command_type = GetAxis;
  command->id = inputID;
  buffsize = sizeof(command->command_type+command->id);
  Wire.beginTransmission(slaveID);
  Wire.write((uint8_t*)i2cBuff, buffsize);
  Wire.endTransmission();
  buffsize += sizeof(uint16_t);
  memset(i2cBuff, 0 ,i2cBuffUsed);
  Wire.requestFrom(slaveID, buffsize);
  for (int i = 0; Wire.available()>0; i++) {
    ((uint8_t*)i2cBuff)[i] = Wire.read();
    i2cBuffUsed = i;
  }
  //i2cBuffUsed = Wire.readBytes((uint8_t*)i2cBuff, buffsize);
  return (uint16_t)(*(uint16_t*)command->data);
}

uint32_t getLedI2cSlave (int LedID, int slaveID) {
  memset(i2cBuff, 0 ,i2cBuffUsed);
  command->command_type = GetLed;
  command->id = LedID;
  buffsize = sizeof(command->command_type+command->id);
  Wire.beginTransmission(slaveID);
  Wire.write((uint8_t*)i2cBuff, buffsize);
  Wire.endTransmission();
  buffsize += sizeof(uint32_t);
  memset(i2cBuff, 0 ,i2cBuffUsed);
  Wire.requestFrom(slaveID, buffsize);
  for (int i = 0; Wire.available()>0; i++) {
    ((uint8_t*)i2cBuff)[i] = Wire.read();
    i2cBuffUsed = i;
  }
  // Serial.print("COMMAND TYPE: ");
  // Serial.print(command->command_type);
  // Serial.print(" GetButton ID: ");
  // Serial.print(command->id);

  // Serial.print(" STATE: ");
  // Serial.println(*(uint32_t*)command->data);
  // Serial.print("BUFF: ");
  // for (int i = 0; i < i2cBuffUsed; i++) {
  //   Serial.print(((uint8_t*)i2cBuff)[i], BIN);
  // }
  // Serial.println();

  return *(uint32_t*)command->data;
}

void setLedI2cSlave (int LedID, int slaveID, uint32_t rgbw) {
  memset(i2cBuff, 0 ,i2cBuffSize);
  command->command_type = SetLed;
  command->id = LedID;
  command->data = (uint32_t*)rgbw;
  buffsize = sizeof(command->command_type+command->id+sizeof(rgbw));
  Wire.beginTransmission(slaveID);
  Wire.write((uint8_t*)i2cBuff, buffsize);
  Wire.endTransmission();
}

// I2C scanner
void i2cScanner() {
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

/*
  =========
    Setup
  =========
 */

void setup() {

  if ( i2cBuff == NULL ) {
    i2cBuff = (uint8_t*)malloc(i2cBuffSize);
    command = (struct Command*)i2cBuff;
  }
  if (_hidKBMReportDesc == nullptr) {
    _hidKBMReportDesc = (uint8_t*)malloc(150);
    memset(_hidKBMReportDesc, 0, 150);
  }
  if (_hidJoystickReportDesc == nullptr) {
    _hidJoystickReportDesc = (uint8_t*)malloc(MAX_HID_REPORTDESC_SIZE);
    memset(_hidJoystickReportDesc, 0, MAX_HID_REPORTDESC_SIZE);
  }


  /* Calculate how many "Devices" we will need to emulate	*/

  // Limitting 8 axis per "Device" since linux WINE doesnt use the 9th axis (and quite a lot of windows games also dont lol)
  DeviceCountJoystick = (int)(std::ceil((float)(AxisCount) / 8)); 
  
  // Limitting 32 buttons per "Device" since older games may not see more (for example Elite Dangerous doesnt see more than 32)
  DeviceCountButtons = (int)(std::ceil((float)(ButtonCount) / 32)); 

  // Limitting 4 hats per "Device" since windows can see 4 max per device (linux only sees 1 per device though... strange)
  DeviceCountHat = (int)(std::ceil((float)(HatCount) / 4)); 

  // Set DeviceCount to the one that is the highest of them all
  if (DeviceCountJoystick >= DeviceCountButtons && DeviceCountJoystick >= DeviceCountHat) {
    DeviceCount = DeviceCountJoystick;
  } else if (DeviceCountButtons >= DeviceCountJoystick && DeviceCountButtons >= DeviceCountHat) {
    DeviceCount = DeviceCountButtons;
  } else {
    DeviceCount = DeviceCountHat;
  }

  for (int d = 0; d < DeviceCount; d++) {
    _hidJoystickReportDescSize = AddHeader(_hidJoystickReportDesc, _hidJoystickReportDescSize, d+1);
    if (AxisCount > 0) {
      _hidJoystickReportDescSize = AddAxis(_hidJoystickReportDesc, _hidJoystickReportDescSize, HID_USAGE_DESKTOP_X, AxisCount, AxisResolution);
      AxisCount -= 8;
      if (AxisCount < 0) {AxisCount = 0;}
    }
    if (ButtonCount > 0) {
      _hidJoystickReportDescSize = AddButtons(_hidJoystickReportDesc, _hidJoystickReportDescSize, ButtonCount);
      ButtonCount -= 32;
      if (ButtonCount < 0) {ButtonCount = 0;}
    }
    if (HatCount > 0) {
      _hidJoystickReportDescSize = AddHats(_hidJoystickReportDesc, _hidJoystickReportDescSize, HatCount);
      HatCount-= 4;
      if (HatCount < 0) {HatCount = 0;}
    }
    _hidJoystickReportDescSize = AddCollectionEnd(_hidJoystickReportDesc, _hidJoystickReportDescSize);
  }


  jr = new hid_Joystick_report_t[DeviceCount];
  jr_old = new hid_Joystick_report_t[DeviceCount];
  memset(jr, 0, sizeof(jr));
  memset(jr_old, 0, sizeof(jr_old));
  readyToSend = (bool*)readyToSend[DeviceCount];


#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as
  // - mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  SerialTinyUSB.begin(115200);
  TinyUSBDevice.setID(VID,PID);
  TinyUSBDevice.setManufacturerDescriptor("Raspberry Pi");
  TinyUSBDevice.setProductDescriptor("RP2040-HOTAS");

  if (USBDevice.ready()||USBDevice.mounted()) {
    USBDevice.detach();
  }
  // start the USB device
   USBDevice.attach();

  if (DeviceCount > 0) {
    hid_joystick.setPollInterval(2);
    hid_joystick.setBootProtocol(HID_ITF_PROTOCOL_NONE);
    hid_joystick.setReportDescriptor(_hidJoystickReportDesc, _hidJoystickReportDescSize);
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
  // wait until device mounted
  //while (!TinyUSBDevice.mounted()){	delay(1); }
  //delay(100);

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
  wifiLoop();
#endif
  if(TinyUSBDevice.ready()) {
    if (micros() - lastTime > 100000) {
      lastTime = micros();
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

  //Serial.println("Reqiest LED 0 data\n\r");
  //command->command_type = GetLed;
  //command->id = 0;
  //size_t size = sizeof(command->command_type+command->id); // only sending command and id (led)
  //Wire.beginTransmission(0x21);
  //Wire.write((byte*)i2cBuff, size);
  //Wire.endTransmission();
  //size += sizeof(uint32_t); //add RGB(W) value from WS2812 to the request
  //// Read from the slave and print out
  //Wire.requestFrom(0x21, size);
  //i2cBuffSizeFree = i2cBuffSize - Wire.readBytes((byte*)i2cBuff, size);
  //led.raw = (uint32_t)command->data;
  //Serial.print("Buff size used: ");
  //Serial.println(i2cBuffSize - i2cBuffSizeFree);
  //Serial.print("LED W: ");
  //Serial.println(led.W);
  //Serial.print("LED R: ");
  //Serial.println(led.R);
  //Serial.print("LED G: ");
  //Serial.println(led.G);
  //Serial.print("LED B: ");
  //Serial.println(led.B);
  // Wake up host if we are in suspend mode
  // and REMOTE_WAKEUP feature is enabled by host
  //if (TinyUSBDevice.suspended() && BOOTSEL)
  //{
  //	TinyUSBDevice.remoteWakeup();
  //}
  

  if (rp2040.fifo.available()) {
    rp2040.fifo.pop_nb((uint32_t*)jr[0].buttons);
  }

  
  if (micros() - 2000 > update_cooldown) {
    update_cooldown = micros();
    if ( readyToSend[report] && 
    //if ( 
    jr[report].x != jr_old[report].x           ||
    jr[report].y != jr_old[report].y           ||
    jr[report].z != jr_old[report].z           ||
    jr[report].rx != jr_old[report].rx         ||
    jr[report].ry != jr_old[report].ry         ||
    jr[report].rz != jr_old[report].rz         ||
    jr[report].slider != jr_old[report].slider ||
    jr[report].dial != jr_old[report].dial     ||
    jr[report].hat1 != jr_old[report].hat1     ||
    jr[report].hat2 != jr_old[report].hat2     ||
    jr[report].buttons != jr_old[report].buttons) {
      if( TinyUSBDevice.ready()) {
        hid_joystick.sendReport(report+1, &jr[report], sizeof(jr[report]));
        readyToSend[report] = false;
        jr_old[report] = jr[report];
      }
      if (TinyUSBDevice.suspended()) {
        TinyUSBDevice.remoteWakeup();
      }
    }
    report++;
    if (report > DeviceCountJoystick)
    report = 0;
  }
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

uint16_t hue;
int nextScan = millis();
int nextScanPrint = millis();
uint32_t buttons;
uint16_t axis;


void setup1()
{

#if defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
  pinMode(23,OUTPUT);
  digitalWrite(23,HIGH); // this forces the switch mode power supply into pwm mode (less efficient but also less noise) 
#endif
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(D0, OUTPUT_2MA);
  pinMode(D1, OUTPUT_2MA);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);
  Wire.setTimeout(10);
  Wire.setSDA(D0);
  Wire.setSCL(D1);
  Wire.setClock(100000);
  Wire.setTimeout(100);
  Wire.begin();

  analogReadResolution(11);
  setLedI2cSlave(0,0x21,0);
  setLedI2cSlave(1,0x21,0);
  setLedI2cSlave(2,0x21,0);
  
  joystick_client.Setup();

#if MAX_UPDATES_PER_SECOND
  static x52::util::RateLimiter<MAX_UPDATES_PER_SECOND, MAX_UPDATES_PER_SECOND> rate_limiter;
#endif

}


void loop1()
{

  //memset(i2cBuff, 0 ,i2cBuffSize);
  if (millis() - nextScan > 100) {
      for (int addr = 0; addr < (1 << 7); ++addr) {

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
  if (millis() - nextScanPrint > 1000) {
    i2cScanner();
    nextScanPrint = millis();
  }
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
    X52DebugPrintln("PollJoystickState failed");
    delayMicroseconds(timeout_micros);
    return;
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
  
  switch (state.pov_1) {
    case 0b0001:  jr[0].hat1 = 0b0011; break;
    case 0b0011:  jr[0].hat1 = 0b0100; break;
    case 0b0010:  jr[0].hat1 = 0b0101; break;
    case 0b0110:  jr[0].hat1 = 0b0110; break;
    case 0b0100:  jr[0].hat1 = 0b0111; break;
    case 0b1100:  jr[0].hat1 = 0b1000; break;
    case 0b1000:  jr[0].hat1 = 0b0001; break;
    case 0b1001:  jr[0].hat1 = 0b0010; break;
    default: jr[0].hat1 = 0b0000; break;
  }
  switch (state.pov_2) {
    case 0b0001:  jr[0].hat2 = 0b0011; break;
    case 0b0011:  jr[0].hat2 = 0b0100; break;
    case 0b0010:  jr[0].hat2 = 0b0101; break;
    case 0b0110:  jr[0].hat2 = 0b0110; break;
    case 0b0100:  jr[0].hat2 = 0b0111; break;
    case 0b1100:  jr[0].hat2 = 0b1000; break;
    case 0b1000:  jr[0].hat2 = 0b0001; break;
    case 0b1001:  jr[0].hat2 = 0b0010; break;
    default: jr[0].hat2 = 0b0000; break;
  }
  jr[0].x = map_clamped<uint16_t>(state.x, 0, 1023, 0, 2047);
  jr[0].y = map_clamped<uint16_t>(state.y, 0, 1023, 0, 2047);
  jr[0].z = map_clamped<uint16_t>(state.z, 0, 1023, 0, 2047);  
  jr[0].rx = map_clamped<uint16_t>(analogRead(A1), 200, 1800, 2047, 0);
  jr[0].ry = map_clamped<uint16_t>(getAxisI2cSlave(1,0x21), 0, 1023, 0, 2047);
  //jr[0].rz = map_clamped<uint16_t>(getAxisI2cSlave(2,0x21), 0, 1023, 0, 2047);
  //jr[0].dial = map_clamped<uint16_t>(getAxisI2cSlave(3,0x21), 0, 1023, 0, 2047);

  //if( rp2040.fifo.push_nb(buttons)) {

    bitWrite(buttons, 0, state.trigger_stage_1);
    bitWrite(buttons, 1, state.button_fire);
    bitWrite(buttons, 2, state.button_a);
    bitWrite(buttons, 3, state.button_b);
    bitWrite(buttons, 4, state.button_c);
    bitWrite(buttons, 5, state.pinkie_switch);
    //bitWrite(buttons, 6, getFromI2cSlave<bool>(GetButton, Button1, 0x21));
    bitWrite(buttons, 6, getButtonI2cSlave(Button1, 0x21));
    //bitWrite(buttons, 7, getFromI2cSlave<bool>(GetButton, Button2, 0x21));
    bitWrite(buttons, 7, getButtonI2cSlave(Button2, 0x21));
    bitWrite(buttons, 8, state.button_t1);
    bitWrite(buttons, 9, state.button_t2);
    bitWrite(buttons, 10, state.button_t3);
    bitWrite(buttons, 11, state.button_t4);
    bitWrite(buttons, 12, state.button_t5);
    bitWrite(buttons, 13, state.button_t6);
    bitWrite(buttons, 14, state.trigger_stage_2);
    bitWrite(buttons, 15, getButtonI2cSlave(Button10, 0x21));
    //bitWrite(buttons, 15, getFromI2cSlave<bool>(GetButton, Button10, 0x21));
    bitWrite(buttons, 16, empty);
    bitWrite(buttons, 17, empty);
    bitWrite(buttons, 18, getButtonI2cSlave(Button11, 0x21));
    //bitWrite(buttons, 18, getFromI2cSlave<bool>(GetButton, Button11, 0x21));    
    bitWrite(buttons, 19, p2u);
    bitWrite(buttons, 20, p2r);
    bitWrite(buttons, 21, p2d);
    bitWrite(buttons, 22, p2l);
    bitWrite(buttons, 23, getButtonI2cSlave(Button6, 0x21));
    //bitWrite(buttons, 23, getFromI2cSlave<bool>(GetButton, Button6, 0x21));
    bitWrite(buttons, 24, getButtonI2cSlave(Button5, 0x21));
    //bitWrite(buttons, 24, getFromI2cSlave<bool>(GetButton, Button5, 0x21));
    bitWrite(buttons, 25, getButtonI2cSlave(Button4, 0x21));
    //bitWrite(buttons, 25, getFromI2cSlave<bool>(GetButton, Button4, 0x21));
    bitWrite(buttons, 26, getButtonI2cSlave(Button7, 0x21));
    //bitWrite(buttons, 26, getFromI2cSlave<bool>(GetButton, Button7, 0x21));
    bitWrite(buttons, 27, M1);
    bitWrite(buttons, 28, M2);
    bitWrite(buttons, 29, M3);
    bitWrite(buttons, 30, getButtonI2cSlave(Button3, 0x21));
    //bitWrite(buttons, 30, getFromI2cSlave<bool>(GetButton, Button3, 0x21));
    bitWrite(buttons, 31, empty);
  //}  
  jr[0].buttons = buttons;

  readyToSend[0] = true;  
#if MAX_UPDATES_PER_SECOND
  }
#endif
}
