#include "Commands.h"
#include "Common.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "CRC.h"
#include "Wire.h"

//int i2cADDR = calcCRC8((uint8_t*)rp2040.getChipID(), sizeof(rp2040.getChipID()));
int i2cADDR = 0x21;
// Buttons
// D E I HAT HAT HAT HAT Scroll Scroll RB LB
// Axis
// ????
#define I2C_SDA 0
#define I2C_SCL 1
#define BUTTON1 2
#define BUTTON2 3
#define BUTTON3 4
#define HAT1_1 5
#define HAT1_2 6
#define HAT1_3 7
#define HAT1_4 8
#define SCROLL_1 9
#define SCROLL_2 10
#define LMB 12
#define RMB 11
#define MUX_SIG A2
#define MUX_S4 A1
#define MUX_S3 A0
#define MUX_S2 22
#define MUX_S1 21
#define MUX_EN 20

#define WS2812_PIN NULL

#define EXT_NUM_PIXELS 3


//Adafruit_NeoPixel ext_pixel(EXT_NUM_PIXELS, WS2812_PIN);

uint8_t ADCresolution = 11;
volatile RGBW led;
uint8_t i2cDataSize = 0;
struct __attribute__((packed, aligned(1))) Command{
  uint16_t command_type;
  uint16_t id;
}command;
uint8_t i2cDataBuf[64];

void receive(int len);
void request();
uint16_t readMuxChannel(uint8_t channel);


///////////////////////////////////////////////
// primary core used for noce specific tasks //
///////////////////////////////////////////////

////////////////////////////////////////////
// secondary core used for communications //
////////////////////////////////////////////

void setup() {
  
#if defined(ARDUINO_RASPBERRY_PI_PICO)||defined(ARDUINO_RASPBERRY_PI_PICO_W) // This changes the SMPS to be less efficient but also less noisy
  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);
#endif

  //ext_pixel.begin();

  analogReadResolution(ADCresolution);

  pinMode(BUTTON1, OUTPUT_2MA);
  pinMode(BUTTON2, OUTPUT_2MA);
  pinMode(BUTTON3, OUTPUT_2MA);

  pinMode(HAT1_1, OUTPUT_2MA);
  pinMode(HAT1_2, OUTPUT_2MA);
  pinMode(HAT1_3, OUTPUT_2MA);
  pinMode(HAT1_4, OUTPUT_2MA);
  pinMode(SCROLL_1, OUTPUT_2MA);
  pinMode(SCROLL_2, OUTPUT_2MA);
  pinMode(LMB, OUTPUT_2MA);
  pinMode(RMB, OUTPUT_2MA);
  pinMode(MUX_EN, OUTPUT_2MA);
  pinMode(MUX_S1, OUTPUT_2MA);
  pinMode(MUX_S2, OUTPUT_2MA);
  pinMode(MUX_S3, OUTPUT_2MA);
  pinMode(MUX_S4, OUTPUT_2MA);
  pinMode(MUX_SIG, INPUT);

  digitalWrite(BUTTON1, HIGH);
  digitalWrite(BUTTON2, HIGH);
  digitalWrite(BUTTON3, HIGH);
  digitalWrite(HAT1_1, HIGH);
  digitalWrite(HAT1_2, HIGH);
  digitalWrite(HAT1_3, HIGH);
  digitalWrite(HAT1_4, HIGH);
  digitalWrite(SCROLL_1, HIGH);
  digitalWrite(SCROLL_2, HIGH);
  digitalWrite(LMB, HIGH);
  digitalWrite(RMB, HIGH);
  digitalWrite(MUX_EN, HIGH);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, LOW);
  digitalWrite(MUX_S4, LOW);

}


void loop() {

}

template <typename T> T readMuxChannel(uint8_t channel) {
  digitalWrite(MUX_S1, ((channel >> 0) & 0x01));
  digitalWrite(MUX_S2, ((channel >> 1) & 0x01));
  digitalWrite(MUX_S3, ((channel >> 2) & 0x01));
  digitalWrite(MUX_S4, ((channel >> 3) & 0x01));
  digitalWrite(MUX_EN, LOW);
  delay(1);
  T value = analogRead(MUX_SIG);
  digitalWrite(MUX_EN, HIGH);
  return (T)value;
}

uint8_t readButton(uint8_t button) {
  bool isPressed = 0;
  switch (button) {
    case Button1:
    { isPressed = digitalRead(BUTTON1); break; }
    case Button2:
    { isPressed = digitalRead(BUTTON2); break; }
    case Button3:
    { isPressed = digitalRead(BUTTON3); break; }
    case Button4:
    { isPressed = digitalRead(HAT1_1); break; }
    case Button5:
    { isPressed = digitalRead(HAT1_2); break; }
    case Button6:
    { isPressed = digitalRead(HAT1_3); break; }
    case Button7:
    { isPressed = digitalRead(HAT1_4); break; }
    case Button8:
    { isPressed = digitalRead(SCROLL_1); break; }
    case Button9:
    { isPressed = digitalRead(SCROLL_2); break; }
    case Button10:
    { isPressed = digitalRead(LMB); break; }
    case Button11:
    { isPressed = digitalRead(RMB); break; }
    case Button12:
    { break; }
    case Button13:
    { break; }
    case Button14:
    { break; }
    case Button15:
    { break; }
    case Button16:
    { break; }
    case Button17:
    { break; }
    case Button18:
    { break; }
    case Button19:
    { break; }
    case Button20:
    { break; }
    case Button21:
    { break; }
    case Button22:
    { break; }
    case Button23:
    { break; }
    case Button24:
    { break; }
    case Button25:
    { break; }
    case Button26:
    { break; }
    default: break;
  }
  // This is since we default our pins high... we pull low when pressed. (this is oposite to how default true/fasle is)
  return !isPressed;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receive(int len) {
  uint16_t command = Wire.read();
  uint16_t id = Wire.read();

  switch (command) {
    case SetLed: {

      //led.raw = (uint32_t)command->data;
      //ext_pixel.setPixelColor(command->id, led.R, led.G, led.B);
      break;
    }
    case SetConfig: {

    }
    case GetConfig: {
      break;
    }
    case GetLed: {
      //((uint32_t*)command.buf)[0] = ext_pixel.getPixelColor(command->id);
      //i2cBuffUsed += sizeof(uint32_t);
      break; 
    }
    case GetAxis: {
      //a = readMuxChannel(command->id);
      //command->data = (uint16_t*)a;

      //((uint16_t*)i2cDataBuf)[0] = readMuxChannel(id);
      ((uint16_t*)i2cDataBuf)[0] = readMuxChannel<uint16_t>(id);
      //((uint16_t*)i2cDataBuf)[0] = readMuxChannel(id,uin);
      i2cDataSize = sizeof(uint16_t);
      break;
    }
    case GetButton: {
      i2cDataBuf[0] = readButton(id);
      i2cDataSize = sizeof(uint8_t);

      //Serial.print("COMMAND TYPE: ");
      //Serial.print(command->command_type);
      //Serial.print(" GetButton ID: ");
      //Serial.print(command->id);
      //Serial.print(" STATE: ");
      //Serial.println((bool)command->data);
      //Serial.print("BUFF: ");
      //for (int i = 0; i < i2cBuffUsed; i++) {
      //  Serial.print(((uint8_t*)i2cBuff)[i], BIN);
      //}
      //Serial.println();

      break;
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


void setup1() {

  //if (i2cBuff == NULL) {
  //  i2cBuff = (uint8_t*)malloc(i2cBuffSize);
  //  command = (struct Command*)i2cBuff;
  //}

  pinMode(D0, OUTPUT_2MA);
  pinMode(D1, OUTPUT_2MA);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);

  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.setTimeout(100, true);
#ifdef I2CSPEED
  //Wire.setClock(I2CSPEED);
#else
  Wire.setClock(1000);
#endif
  Wire.begin(i2cADDR);

  Wire.onReceive(receive);
  Wire.onRequest(request);

}

void loop1() {}