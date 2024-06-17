/*== JOYSTICK ==*/


#define MAX_USB_PACKET_SIZE 64
#define MAX_HID_DESCRIPTOR_SIZE 4096
#define MAX_REPORT_ID 30
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




bool rebootUpdate = false;
bool LED_on;
bool LED_breathe;
uint8_t LED_brightness;
int speed=1;
int LED=0;
uint32_t lastTime = micros();
uint32_t update_cooldown = micros();
int report = 0;

/*== LED ==*/
Adafruit_NeoPixel pixel;

bool enableMouseAndKeyboard = 0;
uint8_t DeviceCountJoystick = 0;
uint8_t DeviceCountButtons = 0;
uint8_t DeviceCountHat = 0;
uint8_t DeviceCount = 0;
int COLLECTIONS = MAX_COLLECTIONS;

uint16_t hue;
uint32_t nextScan = millis();
uint32_t nextScanPrint = millis();
uint16_t axis_start[MAX_REPORT_ID];
uint16_t button_start[MAX_REPORT_ID];
uint16_t hat_start[MAX_REPORT_ID];
uint16_t total_bits[MAX_REPORT_ID];

uint32_t AxisResolution = 11;
uint8_t AxisCount = 8;
uint8_t ButtonCount = 64;
uint8_t HatCount = 2;

uint8_t hid_usage_page_val = HID_USAGE_PAGE_DESKTOP;
uint8_t hid_usage_val = HID_USAGE_DESKTOP_JOYSTICK;

hid_Joystick_report_t* jr = NULL;
hid_Joystick_report_t* jr_old = NULL;
size_t buffsize;
bool* readyToSend = NULL;

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
#ifdef ARDUINO_RASPBERRY_PI_PICO_W
#include "picow_only.h"
extern FS* fileSystem;
#endif


#undef bitSet
#undef bitClear
#define bitSet(value, bit) ((value) |= (1ULL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1ULL << (bit)))


template <typename T>
T map_clamped(T x, T in_min, T in_max, T out_min, T out_max) {
  if (x < in_min) x = in_min;
  if (x > in_max) x = in_max;
  x = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  //if (x > out_max) x = out_max;
  //if (x < out_min) x = out_min;
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
#include "hid_Report_Callback.h"


bool getButtonI2cSlave (int slaveID, int inputID) {
  Wire.beginTransmission(slaveID);
  Wire.write(GetButton);
  Wire.write(inputID);
  Wire.endTransmission();
  buffsize = sizeof(bool);
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


void progressCallBack(size_t currSize, size_t totalSize) {
	  Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
}


void setupUSB() {

  // reset the connection
 TinyUSBDevice.detach();



  /* Calculate how many "Devices" we will need to emulate	*/

  // Limitting 8 axis per "Device" since linux WINE doesnt use the 9th axis (and quite a lot of windows games also dont lol)
  DeviceCountJoystick = (int)(std::ceil((float)(AxisCount) / 8)); 
  
  // Limitting 32 buttons per "Device" since older games may not see more (for example Elite Dangerous doesnt see more than 32)
  DeviceCountButtons = (int)(std::ceil((float)(ButtonCount) / 56)); 

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

  //resetStack();
  uint bufferSize=0;
  uint8_t *buffer = (uint8_t*)malloc(MAX_HID_DESCRIPTOR_SIZE);
  makeDescriptor(1, AxisResolution, AxisCount, HatCount, ButtonCount, buffer, &bufferSize);


  if (jr == NULL)
    jr = new hid_Joystick_report_t[DeviceCount];
  if (jr_old == NULL)
    jr_old = new hid_Joystick_report_t[DeviceCount];
  memset(jr, 0, (uint16_t)sizeof(jr));
  memset(jr_old, 0, (uint16_t)sizeof(jr_old));
  readyToSend = (bool*)readyToSend[DeviceCount];




  TinyUSBDevice.setID(VID,PID);
  TinyUSBDevice.setManufacturerDescriptor("Raspberry Pi");
  TinyUSBDevice.setProductDescriptor("RP2040-HOTAS");

  // start the USB device
  USBDevice.attach();
  SerialTinyUSB.begin(115200);
  
  if (DeviceCount > 0) {
    hid_joystick.setPollInterval(0);
    hid_joystick.setBootProtocol(HID_ITF_PROTOCOL_NONE);
    hid_joystick.setReportDescriptor(buffer, bufferSize);
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
//  memset(hidJoystickReportDesc,0,hidJoystickReportDescSize);
//  free(hidJoystickReportDesc);
  
  // wait until device mounted
  //while (!TinyUSBDevice.mounted()){	delay(1); }
  //delay(100);
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
  //watchdog_enable(2000,true);
}




/*
  ========
    Loop
  ========
 */
void loop()
{
  //watchdog_update();
#ifdef ARDUINO_RASPBERRY_PI_PICO_W
  server.handleClient();
  MDNS.update();

  if (millis() - check_firmware_update > 1000) {
    if (fileSystem->exists("/firmware.bin")) {
      File firmware =  fileSystem->open("/firmware.bin","r");
      if (firmware) {
        Serial.println(F("found!"));
        Serial.println(F("Try to update!"));
        Update.onProgress(progressCallBack);
        Update.begin(firmware.size(), U_FLASH);
        Update.writeStream(firmware);
        if (Update.end()){
          Serial.println(F("Update finished!"));
          rebootUpdate = true;
        }else{
          Serial.println(F("Update error!"));
          Serial.println(Update.getError());
        }
        firmware.close();
        if (fileSystem->rename("/firmware.bin", "/firmware.bak")){
          Serial.println(F("Firmware rename succesfully!"));
        }else{
          Serial.println(F("Firmware rename error!"));
        }
      }
      if (rebootUpdate) {
        rp2040.reboot();
      }
    }
  }
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
  

  //if (rp2040.fifo.available()) {
  //  uint32_t t;
  //  rp2040.fifo.pop_nb(t);
  //}
  
  //if (micros() - update_cooldown > 20) {
    //update_cooldown = micros();
    if ( 
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
        readyToSend[report] = true;
        jr_old[report] = jr[report];
      }
      if (TinyUSBDevice.suspended()) {
        TinyUSBDevice.remoteWakeup();
      }
    }
    report++;
    if (report > DeviceCountJoystick)
      report = 0;
  //}
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
  if (millis() - nextScanPrint > 10000) {
    SerialI2cScanner();
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
  jr[0].x = map_clamped<uint16_t>(deadzone<uint16_t>(state.x,512, 0), 0, 1023, 0, 2047);
  jr[0].y = map_clamped<uint16_t>(deadzone<uint16_t>(state.y,512, 0), 0, 1023, 0, 2047);
  jr[0].z = map_clamped<uint16_t>(deadzone<uint16_t>(state.z,512, 50), 0, 1023, 0, 2047);  //yaw
  jr[0].rx = map_clamped<uint16_t>(analogRead(A1), 300, 1700, 2047, 0);

  // 1 = ??? (nub X?)
  // 2 = ??? (nub Y?)
  // 3 = I dial
  // 4 = E dial
  // 5 = ??? (slider ?)
  //Serial.print(jr[0].rx, BIN);
  //Serial.print(" ");
  //Serial.println(getAxisI2cSlave<uint16_t>(0x21, 5),BIN);

  //jr[0].ry = getAxisI2cSlave(0x21, 5);
  //jr[0].rz = getAxisI2cSlave(0x21, 4);
  //jr[0].dial = getAxisI2cSlave(0x21, 5);

  if (readyToSend[0]) {
    bitWrite(jr[0].buttons, 0, state.trigger_stage_1);
    bitWrite(jr[0].buttons, 1, state.button_fire);
    bitWrite(jr[0].buttons, 2, state.button_a);
    bitWrite(jr[0].buttons, 3, state.button_b);
    bitWrite(jr[0].buttons, 4, state.button_c);
    bitWrite(jr[0].buttons, 5, state.pinkie_switch);
    bitWrite(jr[0].buttons, 6, getButtonI2cSlave(0x21, Button1));
    bitWrite(jr[0].buttons, 7, getButtonI2cSlave(0x21, Button3));
    bitWrite(jr[0].buttons, 8, state.button_t1);
    bitWrite(jr[0].buttons, 9, state.button_t2);
    bitWrite(jr[0].buttons, 10, state.button_t3);
    bitWrite(jr[0].buttons, 11, state.button_t4);
    bitWrite(jr[0].buttons, 12, state.button_t5);
    bitWrite(jr[0].buttons, 13, state.button_t6);
    bitWrite(jr[0].buttons, 14, state.trigger_stage_2);
    bitWrite(jr[0].buttons, 15, getButtonI2cSlave(0x21, Button10));
    bitWrite(jr[0].buttons, 16, empty); // scroll up
    bitWrite(jr[0].buttons, 17, empty); //scroll down
    bitWrite(jr[0].buttons, 18, getButtonI2cSlave(0x21, Button11));
    bitWrite(jr[0].buttons, 19, p2u);
    bitWrite(jr[0].buttons, 20, p2r);
    bitWrite(jr[0].buttons, 21, p2d);
    bitWrite(jr[0].buttons, 22, p2l);
    bitWrite(jr[0].buttons, 23, getButtonI2cSlave(0x21, Button7));
    bitWrite(jr[0].buttons, 24, getButtonI2cSlave(0x21, Button6));
    bitWrite(jr[0].buttons, 25, getButtonI2cSlave(0x21, Button5));
    bitWrite(jr[0].buttons, 26, getButtonI2cSlave(0x21, Button4));
    bitWrite(jr[0].buttons, 27, M1);
    bitWrite(jr[0].buttons, 28, M2);
    bitWrite(jr[0].buttons, 29, M3);
    bitWrite(jr[0].buttons, 30, getButtonI2cSlave(0x21, Button2));

  // 15/14 scroll mfd
  // 13 mfd select
  // 12 reset
  // 11 start/stop
  // 10 function
  // 9/8 pg scroll


    bitWrite(jr[0].buttons, 31, !digitalRead(10)); //function
    bitWrite(jr[0].buttons, 32, !digitalRead(11)); // start/stop
    bitWrite(jr[0].buttons, 33, !digitalRead(12)); //reset
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
    bitWrite(jr[0].buttons, 37, !digitalRead(13)); // mfd select
    readyToSend[0] = false;  
  };
  //if (!digitalRead(10)||!digitalRead(11)||!digitalRead(12)||!digitalRead(13)) {digitalWrite(LED_BUILTIN,1);}else {digitalWrite(LED_BUILTIN,0);}  //if( rp2040.fifo.push_nb(buttons))
  //{
  //}  
  //jr[0].buttons = buttons;
  
#if MAX_UPDATES_PER_SECOND
  }
#endif
}
