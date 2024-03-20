#include "Commands.h"
#include "Common.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "CRC.h"
#include "Wire.h"


int i2cADDR = calcCRC8((uint8_t*)rp2040.getChipID(), sizeof(rp2040.getChipID()));

#define I2C_SDA D0
#define I2C_SCL D1
#define WS2812_EXT D2
#define MUX_EN D3
#define MUX_S4 D4
#define MUX_S3 D5
#define MUX_S2 D6
#define MUX_S1 D7

#if defined(ARDUINO_WAVESHARE_RP2040_ZERO)
#define MUX_SIG A3
#elif defined(ARDUINO_RASPBERRY_PI_PICO)
#define MUX_SIG A2
#endif

#define BUTTON1 D8
#define BUTTON2 D9
#define BUTTON3 D2
#define HAT1_1 D11
#define HAT1_2 D12
#define HAT1_3 D13
#define HAT1_4 D14
#define LMB D15
#define WS2812_INT D16
#define RMB D25
#define SCROLL_1 D26
#define SCROLL_2 D27

#define INT_NUM_PIXELS 1
#define EXT_NUM_PIXELS 3


Adafruit_NeoPixel ext_pixel(EXT_NUM_PIXELS, WS2812_EXT);
Adafruit_NeoPixel int_pixel(INT_NUM_PIXELS, WS2812_INT);

volatile Command* command;
volatile RGBW led;
uint8_t i2cBuffSize = 255;
uint8_t i2cBuffSizeFree = i2cBuffSize;
volatile void* i2cBuff = NULL;
volatile bool NewData = false;
static char buff[100];

void receive(int len);
void request();
uint16_t readMuxChannel(uint8_t channel);


void setup() {
	if (i2cBuff == NULL) {
		i2cBuff = (uint8_t*)malloc(i2cBuffSize);
	}
	command = (struct Command*)i2cBuff;

	
#if defined(ARDUINO_RASPBERRY_PI_PICO) // This changes the SMPS to be less efficient but also less noisy
	pinMode(23, OUTPUT);
	digitalWrite(23, HIGH);
#endif

	Serial.begin(115200);
	ext_pixel.begin();

	Wire.setSDA(I2C_SDA);
	Wire.setSCL(I2C_SCL);
	Wire.setClock(100000);
	Wire.begin(i2cADDR);

	Wire.onReceive(receive);
	Wire.onRequest(request);

	analogReadResolution(11);
}
void loop() {
	if (ext_pixel.canShow())
		ext_pixel.show();
}


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


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receive(int len) {
  i2cBuffSizeFree = i2cBuffSize - Wire.readBytes((byte*)i2cBuff, len);
	switch (command->command_type) {
		case SET_LED: {
			led.raw = (uint32_t)command->data;
			ext_pixel.setPixelColor(command->id, led.R, led.G, led.B);
			break;
		}
		case SET_AXIS: {
			break;
		}
		case SET_BUTTON: {
			break;
		}
		default: {
			break;
		}
	}
}

// function that executes whenever data is received from master
// Called when the I2C slave is read from
void request() {
//	static int ctr = 765;
//	char buff[7];
//	// Return a simple incrementing hex value
//	sprintf(buff, "%06X", (ctr++) % 65535);
//	Wire.write(buff, 6);
	switch (command->command_type) {
		case GET_CONFIG: {
			break;
		}
		case GET_LED: {
			command->data = (uint32_t*) ext_pixel.getPixelColor(command->id);
			i2cBuffSizeFree -= sizeof(uint32_t);
			break; 
		}
		case GET_AXIS: {
			break;
		}
		case GET_BUTTON: {
			break;
		}
		default: {
			break;
		}
	}
	Wire.write((byte*)i2cBuff, i2cBuffSize - i2cBuffSizeFree);
}


int i2 = 0;
void setup1() {
	int_pixel.begin();
}

void loop1() {
	if (int_pixel.canShow()) {
		int_pixel.rainbow(i2,1,255,128,true);
		int_pixel.show();
		i2+=10;
		if (i2 > UINT16_MAX)
			i2 = 0;
	}
	delay(1);
}