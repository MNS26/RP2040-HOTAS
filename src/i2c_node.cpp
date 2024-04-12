#include "Commands.h"
#include "Common.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "CRC.h"
#include "Wire.h"
int i2cADDR = calcCRC8((uint8_t*)rp2040.getChipID(), sizeof(rp2040.getChipID()));

#define I2C_SDA 0
#define I2C_SCL 1
#define WS2812_EXT 2
#define WS2812_INT NULL
#define BUTTON1 3
#define BUTTON2 4
#define BUTTON3 5
#define HAT1_1 6
#define HAT1_2 7
#define HAT1_3 8
#define HAT1_4 9
#define SCROLL_1 10
#define SCROLL_2 11
#define LMB 12
#define RMB 13
#define MUX_SIG 28
#define MUX_EN 27
#define MUX_S4 26
#define MUX_S3 22
#define MUX_S2 21
#define MUX_S1 20


#define EXT_NUM_PIXELS 3


Adafruit_NeoPixel ext_pixel(EXT_NUM_PIXELS, WS2812_EXT);

volatile Command* command;
volatile RGBW led;
uint8_t i2cBuffSize = 255;
uint8_t i2cBuffSizeFree = i2cBuffSize;
uint8_t i2cBuffUsed;
void* i2cBuff = NULL;


void receive(int len);
void request();
uint16_t readMuxChannel(uint8_t channel);


void setup() {
  
#if defined(ARDUINO_RASPBERRY_PI_PICO)||defined(ARDUINO_RASPBERRY_PI_PICO_W) // This changes the SMPS to be less efficient but also less noisy
  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);
#endif

  Serial.begin(115200);
  ext_pixel.begin();

  analogReadResolution(11);

  pinMode(BUTTON1, OUTPUT_2MA);
  digitalWrite(BUTTON1, HIGH);
  pinMode(BUTTON2, OUTPUT_2MA);
  digitalWrite(BUTTON2, HIGH);
  pinMode(BUTTON3, OUTPUT_2MA);
  digitalWrite(BUTTON3, HIGH);

  pinMode(HAT1_1, OUTPUT_2MA);
  digitalWrite(HAT1_1, HIGH);
  pinMode(HAT1_2, OUTPUT_2MA);
  digitalWrite(HAT1_2, HIGH);
  pinMode(HAT1_3, OUTPUT_2MA);
  digitalWrite(HAT1_3, HIGH);
  pinMode(HAT1_4, OUTPUT_2MA);
  digitalWrite(HAT1_4, HIGH);

  pinMode(SCROLL_1, OUTPUT_2MA);
  digitalWrite(SCROLL_1, HIGH);
  pinMode(SCROLL_2, OUTPUT_2MA);
  digitalWrite(SCROLL_2, HIGH);

  pinMode(LMB, OUTPUT_2MA);
  digitalWrite(LMB, HIGH);
  pinMode(RMB, OUTPUT_2MA);
  digitalWrite(RMB, HIGH);



}

void loop() {
  if (ext_pixel.canShow())
    ext_pixel.show();
    

}
uint16_t a = 2000;

uint16_t readMuxChannel(uint8_t channel) {
  digitalWriteFast(MUX_S1, bitRead(channel,0));
  digitalWriteFast(MUX_S2, bitRead(channel,1));
  digitalWriteFast(MUX_S3, bitRead(channel,2));
  digitalWriteFast(MUX_S4, bitRead(channel,3));
  digitalWriteFast(MUX_EN, LOW);
  uint16_t value = analogRead(MUX_SIG);
  digitalWriteFast(MUX_EN, HIGH);
  return value;
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
  memset(i2cBuff, 0, i2cBuffSize);
  i2cBuffUsed = Wire.readBytes((uint8_t*)i2cBuff, len);

  switch (command->command_type) {
    case SetLed: {
      led.raw = (uint32_t)command->data;
      ext_pixel.setPixelColor(command->id, led.R, led.G, led.B);
      break;
    }
    case SetAxis: {
      break;
    }
    case SetButton: {
      break;
    }
    case GetConfig: {
      break;
    }
    case GetLed: {
      command->data = (uint32_t*)ext_pixel.getPixelColor(command->id);
      i2cBuffUsed += sizeof(uint32_t);
      break; 
    }
    case GetAxis: {
      //command->data = (uint16_t*)a;
      command->data = (uint16_t*)readMuxChannel(command->id);
      i2cBuffUsed += sizeof(uint16_t);
      break;
    }
    case GetButton: {
      command->data = (uint8_t*)readButton(command->id);
      i2cBuffUsed += sizeof(uint8_t);

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
    
  Wire.write((uint8_t*)i2cBuff, i2cBuffUsed);
}


void setup1() {

  if (i2cBuff == NULL) {
    i2cBuff = (uint8_t*)malloc(i2cBuffSize);
  }
  command = (struct Command*)i2cBuff;

  pinMode(D0, OUTPUT_2MA);
  pinMode(D1, OUTPUT_2MA);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);

  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.setClock(100000);
  Wire.begin(i2cADDR);

  Wire.onReceive(receive);
  Wire.onRequest(request);

}

void loop1() {}