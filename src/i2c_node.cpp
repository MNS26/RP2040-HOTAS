#include "Commands.h"
#include "Common.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "CRC.h"
#include "Wire.h"
#include <EEPROM.h>

// Buttons
// D E I HAT HAT HAT HAT Scroll Scroll RB LB
// Axis
// ????
#define I2C_SDA 0
#define I2C_SCL 1

#define WS2812_PIN NULL

#define EXT_NUM_PIXELS 3
uint8_t maxHats = 8;
uint8_t maxAxis = 32;
uint8_t maxButtons = 32;
uint8_t maxMuxes = 5;

//int i2cADDR = calcCRC8((uint8_t*)rp2040.getChipID(), sizeof(rp2040.getChipID()));
struct _config {
  int i2cADDR = 0x21;
  uint8_t ADCResolution = 11;
  uint8_t currentADCResolution;
  uint8_t digitalPinCount = 16;
  uint8_t analogPinCount = 1;
  uint8_t muxCount = 1;
  uint8_t axisCount = 3;
  uint8_t buttonCount = 7;
  uint8_t hatCount = 1;
  // pin ids used for reading
  uint8_t digitalPins[32]; 
  uint8_t analogPins[32];
  // pin modes (INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN, OUTPUT_2MA, OUTPUT_4MA, OUTPUT_8MA, OUTPUT_12MA)
  uint8_t digitalModePins[32];
  uint8_t analogModePins[32];

  uint8_t buttonPins[32];
  uint8_t bInverted[32];
  uint8_t axisPins[32];
  uint8_t aInverted[32];
  // order is: N,E,S,W
  uint8_t hatPins[8][4];
  // order is: EN,S0,S1,S2,S3,SIG
  uint8_t muxPins[5][6];
};
_config config;
Adafruit_NeoPixel ext_pixel(EXT_NUM_PIXELS, WS2812_PIN);
volatile RGBW led;

uint8_t i2cDataSize = 0;
uint8_t i2cDataBuf[64];

void readEEPROM() {
  //EEPROM.get(0,config);
  for (uint8_t i = 0; i<sizeof(config);i++) {
    ((uint8_t*)&config)[i] = EEPROM[i];
  }
}

void writeEEPROM() {
  //    EEPROM.put(0,config);
  for (uint8_t i = 0; i<sizeof(config);i++) {
    if (((uint8_t*)&config)[i] != EEPROM[i])
      EEPROM[i] = ((uint8_t*)&config)[i];
  }
}

void commitEEPROM() {
  EEPROM.commit();
}

// Reads the analog pin
// muxId is 0 its reading a analog pin directly
// otherwise muxId is used to get the pins from the array for that mux
// and channel is used to select the pin instead
template <typename T> T readAxis(uint8_t muxId, uint8_t channel) {
  T value = 0;
  if(muxId>0 && muxId <= config.muxCount) {
    digitalWrite(config.muxPins[muxId][1], ((channel >> 0) & 0x01));
    digitalWrite(config.muxPins[muxId][2], ((channel >> 1) & 0x01));
    digitalWrite(config.muxPins[muxId][3], ((channel >> 2) & 0x01));
    digitalWrite(config.muxPins[muxId][4], ((channel >> 3) & 0x01));
    digitalWrite(config.muxPins[muxId][0], LOW);
    value = analogRead(config.muxPins[muxId][5]);
    digitalWrite(config.muxPins[muxId][0], HIGH);
//    digitalWrite(MUX_S1, ((channel >> 0) & 0x01));
//    digitalWrite(MUX_S2, ((channel >> 1) & 0x01));
//    digitalWrite(MUX_S3, ((channel >> 2) & 0x01));
//    digitalWrite(MUX_S4, ((channel >> 3) & 0x01));
//    digitalWrite(MUX_EN, LOW);
//    delay(1);
//    value = analogRead(MUX_SIG);
//    digitalWrite(MUX_EN, HIGH);
  } else {
    value = analogRead(channel);
  }
  return (T)value;
}

bool readButton(uint8_t muxId, uint8_t button) {
// This is since we default our pins high... we pull low when pressed. (this is oposite to how default true/fasle is)
  bool isPressed = 0;
  if(muxId>0 && muxId <= config.muxCount) {
    digitalWrite(config.muxPins[muxId][1], ((button >> 0) & 0x01));
    digitalWrite(config.muxPins[muxId][2], ((button >> 1) & 0x01));
    digitalWrite(config.muxPins[muxId][3], ((button >> 2) & 0x01));
    digitalWrite(config.muxPins[muxId][4], ((button >> 3) & 0x01));
    digitalWrite(config.muxPins[muxId][0], LOW);
    isPressed = !digitalRead(config.muxPins[muxId][5]);
    digitalWrite(config.muxPins[muxId][0], HIGH);
  }
  else {
    isPressed = !digitalRead(config.digitalPins[button]);
  }
  return isPressed;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receive(int len) {
  uint8_t command = Wire.read();
  switch (command) {
    case SetLed: {
      //uint8_t id = Wire.read();
      led.raw = Wire.read()|(Wire.read()<<8)|(Wire.read()<<16)|(Wire.read()<<24);
      //ext_pixel.setPixelColor(id, led.R, led.G, led.B);
      break;
    }
    case GetLed: {
      //uint8_t id = Wire.read();
      //((uint32_t*)i2cDataBuf)[0] = ext_pixel.getPixelColor(id);
      //i2cBuffUsed += sizeof(uint32_t);
      break; 
    }
    case GetAxis: {
      uint8_t id = Wire.read();
      uint8_t muxId = 0;
      if (Wire.available())
        muxId = Wire.read();
      ((uint16_t*)i2cDataBuf)[0] = readAxis<uint16_t>(muxId, id);
      i2cDataSize = sizeof(uint16_t);
      break;
    }
    case GetButton: {
      uint8_t id = Wire.read();
      uint8_t muxId = 0;
      if (Wire.available())
        muxId = Wire.read();
      i2cDataBuf[0] = readButton(muxId, id);
      i2cDataSize = sizeof(uint8_t);
      break;
    }
    case  Config: {
      uint16_t configCommand = Wire.read()|(Wire.read()<<8);
      switch (configCommand) {
        case ConfigUpdate: {
          break;
        }
        case SetI2cId: {
          uint8_t id = Wire.read();
          if (config.i2cADDR != id) {
            config.i2cADDR = id;
          }
          break;
        }
        case GetI2cId: {
          i2cDataBuf[0] = config.i2cADDR;
          i2cDataSize = 1;
          break;
        }
        case SetAnalogResolution: {
          config.ADCResolution = Wire.read();
          if (config.currentADCResolution != config.ADCResolution) {
            analogReadResolution(config.ADCResolution);
            config.currentADCResolution = config.ADCResolution;
          }
          break;
        }
        case SetButtonPin: {
          uint8_t id = Wire.read();
          uint8_t pin = Wire.read();
          if (id < maxButtons)
            config.digitalPins[id] = pin;
          break;
        }
        case GetButtonPin: {
          uint8_t id = Wire.read();
          i2cDataBuf[0] = config.digitalPins[id];
          i2cDataSize = sizeof(uint8_t);
          break;
        }
        case SetAxisPin: {
          uint8_t id = Wire.read();
          uint8_t pin = Wire.read();
          if (id < maxAxis)
            config.analogPins[id] = pin;
          break;
        }
        case GetAxisPin: {
          uint8_t id = Wire.read();
          i2cDataBuf[0] = config.analogPins[id];
          i2cDataSize = sizeof(uint8_t);
          break;
        }
        case SetHatPins: {
          uint8_t id = Wire.read();
          if (id < maxHats)
            for (uint8_t i = 0; i< 4; i++) {
              config.hatPins[id][i] = Wire.read();
            }
          break;
        }
        case GetHatPins: {
          uint8_t id = Wire.read();
          if (id < maxHats) {
            memcpy(i2cDataBuf, config.hatPins[id], sizeof(config.hatPins[id]));
            i2cDataSize = sizeof(config.hatPins[id]);
          }
          break;
        }
        case SetMuxPins: {
          uint8_t id = Wire.read();
          if (id < maxMuxes)
            for (uint8_t i = 0; i< 6; i++) {
              config.muxPins[id][i] = Wire.read();
            }
          break;
        }
        case GetMuxPins: {
          uint8_t id = Wire.read();
          if (id < maxMuxes) {
            memcpy(i2cDataBuf, config.muxPins[id], sizeof(config.muxPins[id]));
            i2cDataSize = sizeof(config.muxPins[id]);
          }
          break;
        }
        case SetButtonCount: {
          uint8_t count = Wire.read();
          if (count < maxButtons)
            config.buttonCount = count;
          break;
        }
        case GetButtonCount: {
          i2cDataBuf[0] = config.buttonCount;
          i2cDataSize = 1;
          break;
        }
        case SetHatCount: {
          uint8_t count = Wire.read();
          if (count < maxHats)
            config.hatCount = count;
          break;
        }
        case GetHatCount: {
          i2cDataBuf[0] = config.hatCount;
          i2cDataSize = 1;
          break;
        }
        case SetAxisCount: {
          uint8_t count = Wire.read();
          if (count < maxAxis)
            config.axisCount = count;
          break;
        }
        case GetAxisCount: {
          i2cDataBuf[0] = config.axisCount;
          i2cDataSize = 1;
          break;
        }
        case SetMuxCount: {
          uint8_t count = Wire.read();
          if (count < maxMuxes)
            config.muxCount = count;
          break;
        }
        case GetMuxCount: {
          i2cDataBuf[0] = config.muxCount;
          i2cDataSize = 1;
          break;
        }
        case GetAnalogResolution: {
          i2cDataBuf[0] = config.currentADCResolution;
          i2cDataSize = sizeof(uint8_t);
          break;
        }
        default:
          break;
      }
    }
    default:
    break;
  }
}

// function that executes whenever data is received from master
// Called when the I2C slave is read from
void request() {
  Wire.write(i2cDataBuf, i2cDataSize);
}


void setup() {
  
// This changes the SMPS to be less efficient but also less noisy
#if defined(ARDUINO_RASPBERRY_PI_PICO)||defined(ARDUINO_RASPBERRY_PI_PICO_W) 
  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);
#endif
  // since its not actually a eeprom we MUST only write to it when we REALLY have to
  EEPROM.begin(512);
  ////////////////////////////
  //     EEPROM layout:     //
  ////////////////////////////
  // (1)  i2c address       //
  // (1)  adc resolution    //
  // (1)  digital pin count //
  // (1)  analog pin count  //
  // (1)  mux count         //
  // (1)  rgb led count     //
  // (1)  rgb led pin       //
  // (32) digital pins      //
  // (32) analog pins       //
  // (32) mux pins          //
  ////////////////////////////

  auto a = sizeof(config);
  
  uint8_t digitalPins[] = {2,3,4,5,6,7,8,9,10,11,12,20,21,22,26,27};
  uint8_t digitalPinMode[] = {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};
  uint8_t analogPins[] = {28};
  uint8_t analogPinMode[]  = {0};
  uint8_t buttonPins[] = {2,3,4,5,6,7,8,9,10,11,12,20,21,22,26,27};
  uint8_t axisPins[]  = {28};
  uint8_t hatPins[8][4]  = {{5,6,7,8}};
  uint8_t muxPins[5][6] = {{20,21,22,26,27,28}};

  memcpy(config.digitalPins, digitalPins, sizeof(digitalPins));
  memcpy(config.digitalModePins, digitalPinMode, sizeof(digitalPinMode));
  memcpy(config.analogPins, analogPins, sizeof(analogPins));
  memcpy(config.analogModePins, analogPinMode, sizeof(analogPinMode));
  memcpy(config.buttonPins, buttonPins, sizeof(buttonPins));
  memcpy(config.axisPins, axisPins, sizeof(axisPins));
  memcpy(config.hatPins, hatPins, sizeof(hatPins));
  memcpy(config.muxPins, muxPins, sizeof(muxPins));

  //ext_pixel.begin();

  analogReadResolution(config.currentADCResolution);

  for (uint8_t i = 0; i <config.digitalPinCount; i++) {
    pinMode(config. digitalPins[i],config.digitalModePins[i]);
    digitalWrite(config.digitalPins[i],HIGH);
  }
  for (uint8_t i = 0; i <config.analogPinCount; i++) {
    pinMode(config.analogPins[i],  config.analogModePins[i]);
  }
  
  //pinMode(I2C_SDA, OUTPUT_2MA);
  //pinMode(I2C_SCL, OUTPUT_2MA);
  //digitalWrite(I2C_SDA, HIGH);
  //digitalWrite(I2C_SCL, HIGH);

  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.setTimeout(10, true);
  Wire.begin(config.i2cADDR);
  Wire.onReceive(receive);
  Wire.onRequest(request);

}


void loop() {

}
