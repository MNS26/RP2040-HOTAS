/*== JOYSTICK ==*/
#include "Commands.h"
#include "Common.h"
#include "hid.h"
#include "HID_descriptors.hpp"
#include "HID_Report.hpp"
#include <Adafruit_TinyUSB.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "CRC.h"
#include "Wire.h"
#define MAX_HID_REPORTDESC_SIZE 5120

/*== LED ==*/
Adafruit_NeoPixel pixel;

bool enableMouseAndKeyboard = 0;
uint8_t JS_COUNT;
int COLLECTIONS = MAX_COLLECTIONS;

int AXIS_COUNT = 5;

hid_Joystick_report_t* jr = nullptr;
hid_Joystick_report_t* jr_old = nullptr;
uint8_t* _hidJoystickReportDesc = nullptr;
uint16_t _hidJoystickReportDescSize = 0;
uint8_t* _hidKBMReportDesc = nullptr;
uint16_t _hidKBMReportDescSize = 0;

// Report ID
enum RID{
	RID_JOYSTICK1 = 1,
	RID_JOYSTICK2,
};

//DATA TYPE
enum
{
	Axis = 1,
	Buttons,
	Hat,
	Padding,
}; 

// HID report descriptor using TinyUSB's template
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD( VA_HID_REPORT_ID(1) ),
  TUD_HID_REPORT_DESC_MOUSE   ( VA_HID_REPORT_ID(2) ),
};


//Adafruit_USBD_HID hid(desc_hid_report, sizeof(desc_hid_report),HID_ITF_PROTOCOL_NONE,2,true);
Adafruit_USBD_HID hid_joystick;
Adafruit_USBD_HID hid_kbm;

uint8_t i2cBuffSize = 255;
uint8_t i2cBuffSizeFree = i2cBuffSize;

void* i2cBuff = nullptr;
volatile RGBW led;
volatile Command* command;

template <typename T>
T map_clamped(T x, T in_min, T in_max, T out_min, T out_max)
{
	if (x > in_max)
		x = in_max;
	if (x < in_min)
		x = in_min;
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
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
	switch (report_type)
	{
		case (hid_report_type_t)HID_REPORT_TYPE_INVALID:
		case (hid_report_type_t)HID_REPORT_TYPE_OUTPUT:
		break;

		case (hid_report_type_t)HID_REPORT_TYPE_INPUT:
		case (hid_report_type_t)HID_REPORT_TYPE_FEATURE:	
			{
				Serial.print("get_report type: ");
				Serial.println((int)report_type);
				Serial.print("get_report id: ");
				Serial.println((int)report_id);
		
				//for (int i = 0; i<reqlen; i++) {	
				//	buffer[i] = 0x00;
				//}
				if (report_id == 0){
					for (int i = 0; i <JS_COUNT; i++)
					{
					buffer[0] = i+1;
					buffer[i+1] = i+1;
//					buffer[2] = RID_JOYSTICK2;
					}
				}
				if (report_id == RID_JOYSTICK1) {
				

					//buffer[0] = 9;

					buffer[1] = Buttons;
					buffer[2] = 1;
					buffer[3] = 32;

					for (int i = 0; i < AXIS_COUNT; i++) {
						buffer[(i*6)+4]  = Axis;
						buffer[(i*6)+5]  = 11;
						buffer[(i*6)+7]  = 1;
						buffer[(i*6)+8]  = Padding;
						buffer[(i*6)+9]  = 5;
						buffer[(i*6)+10] = 1;					
					}
					//buffer[4] = Axis;
					//buffer[5] = 11;
					//buffer[6] = 1;

					//buffer[7] = Padding;
					//buffer[8] = 5;
					//buffer[9] = 1;
					buffer[0] = sizeof(buffer)-1;
				}
				if (report_id == RID_JOYSTICK2){
					//buffer[0] = 9;

					buffer[1] = Buttons;
					buffer[2] = 1;
					buffer[3] = 32;

					for (int i = 0; i < AXIS_COUNT; i++) {
						buffer[(i*6)+4]  = Axis;
						buffer[(i*6)+5]  = 11;
						buffer[(i*6)+7]  = 1;
						buffer[(i*6)+8]  = Padding;
						buffer[(i*6)+9]  = 5;
						buffer[(i*6)+10] = 1;					
					}
					//buffer[4] = Axis;
					//buffer[5] = 11;
					//buffer[6] = 1;

					//buffer[7] = Padding;
					//buffer[8] = 5;
					//buffer[9] = 1;
					buffer[0] = sizeof(buffer)-1;
				}
				//if (report_id == RID_KEYBOARD)
				//{
				//	for (int i = 0; i<reqlen; i++) {	
				//		buffer[i] = 0xf0;
				//	}
				//}
				//if (report_id == RID_MOUSE)
				//{
				//	for (int i = 0; i<reqlen; i++) {	
				//		buffer[i] = 0x0f;
				//	}
				//}
			}
		return reqlen;
	}
	return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
	Serial.print("rep ID: ");
	Serial.println(report_id);
	//for (int i = 0; i<bufsize;i++)
	//{
	//	Serial.print((char)buffer[i]);
	//}
	switch (report_type)
	{
		case HID_REPORT_TYPE_INVALID:
		case HID_REPORT_TYPE_INPUT:
		case HID_REPORT_TYPE_OUTPUT:
		break;

		case HID_REPORT_TYPE_FEATURE:
			{
				Serial.print("set_report type: ");
				Serial.println((int)report_type);
				if (report_type == HID_REPORT_TYPE_FEATURE && report_id == RID_JOYSTICK1) {
					
				}
				if (report_type == HID_REPORT_TYPE_FEATURE && report_id == RID_JOYSTICK2) {
					
				}
				//if (report_type == HID_REPORT_TYPE_FEATURE && report_id == RID_KEYBOARD) {
				//	
				//}
			}
		break;
	}
	//usb.sendReport(report_id,buffer,bufsize);

}


// I2C scanner
void i2cScanner() {
	Serial.print("\n\rI2C Bus Scan\n\r");
	Serial.print("    0 1 2 3 4 5 6 7 8 9 A B C D E F\n\r");

	for (int addr = 0; addr < (1 << 7); ++addr) {
		if (addr % 16 == 0) {
			Serial.printf("%02x: ", addr);
		}
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

void setup()
{
	rp2040.enableDoubleResetBootloader();

	JS_COUNT = static_cast<int>( std::ceil( static_cast<float>(AXIS_COUNT)/8 ) );

	if (_hidKBMReportDesc == nullptr) {
		_hidKBMReportDesc = (uint8_t*)malloc(150);
		memset(_hidKBMReportDesc, 0, 150);
	}
	if (_hidJoystickReportDesc == nullptr) {
		_hidJoystickReportDesc = (uint8_t*)malloc(MAX_HID_REPORTDESC_SIZE);
		memset(_hidJoystickReportDesc, 0, MAX_HID_REPORTDESC_SIZE);
	}
	if ( i2cBuff == nullptr ) {
		i2cBuff = (uint8_t*)malloc(i2cBuffSize);
		command = (struct Command*)i2cBuff;
	}


	jr = new hid_Joystick_report_t[JS_COUNT];
	jr_old = new hid_Joystick_report_t[JS_COUNT];
	memset(jr, 0, sizeof(jr));
	memset(jr_old, 0, sizeof(jr_old));

	for (int i = 0; i <JS_COUNT; i++) {
		memcpy(_hidJoystickReportDesc+(sizeof(HID_JOYSTICK_rep)*i), HID_JOYSTICK_rep, sizeof(HID_JOYSTICK_rep));
		// set the ID of report_id directly. It does this by calulating the collection offest with (report size * i) and adds + 7 to get the exact index
		_hidJoystickReportDesc[_hidJoystickReportDescSize+7] = i+1;		
		_hidJoystickReportDescSize += sizeof(HID_JOYSTICK_rep);
	}

	//if (enable_keyboard) {
	//	// Jank but works... couldnt find another way that worked
	//	uint8_t const desc_hid_report1[] = {TUD_HID_REPORT_DESC_KEYBOARD(VA_HID_REPORT_ID(JS_COUNT+1))};
	//	memcpy(_hidKBMReportDesc+_hidKBMReportDescSize, desc_hid_report1, sizeof(desc_hid_report1));
	//	//_hidKBMReportDesc[_hidKBMReportDescSize+7] = 1;
	//	_hidKBMReportDescSize += sizeof(desc_hid_report1);
	//}

	//if (enable_mouse) {
	//	// Jank but works... couldnt find another way that worked
	//	uint8_t const desc_hid_report2[] = {TUD_HID_REPORT_DESC_MOUSE(VA_HID_REPORT_ID(JS_COUNT+1))};
	//	memcpy(_hidKBMReportDesc+_hidKBMReportDescSize, desc_hid_report2, sizeof(desc_hid_report2));
	//	//_hidKBMReportDesc[_hidKBMReportDescSize+7] = 2;
	//	_hidKBMReportDescSize += sizeof(desc_hid_report2);
	//}

#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
	// Manual begin() is required on core without built-in support for TinyUSB such as
	// - mbed rp2040
	TinyUSB_Device_Init(0);
#endif
	SerialTinyUSB.begin(115200);

	TinyUSBDevice.setID(VID,PID);
	TinyUSBDevice.setManufacturerDescriptor("Raspberry Pi");
	TinyUSBDevice.setProductDescriptor("EDGC");
	
	// start the USB device
 	USBDevice.attach();

	if (JS_COUNT > 0) {
		//hid.setPollInterval(2);
		//hid_joystick.setBootProtocol(HID_ITF_PROTOCOL_NONE);
		//hid_joystick.setStringDescriptor("TinyUSB Joystick");
		hid_joystick.setReportDescriptor(_hidJoystickReportDesc, _hidJoystickReportDescSize);
		hid_joystick.setReportCallback(get_report_callback, set_report_callback);
		hid_joystick.begin();
	}
	if (enableMouseAndKeyboard) {
		hid_kbm.setStringDescriptor("TinyUSB Keyboard/Mouse");
		hid_kbm.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
		hid_kbm.begin();
	}
	// wait until device mounted
	//while (!TinyUSBDevice.mounted()){	delay(1); }
	//delay(100);

	#if defined(ARDUINO_RASPBERRY_PI_PICO)
	pinMode(23,OUTPUT);
	digitalWrite(23,HIGH); // this forces the switch mode power supply into pwm mode (less efficient but also less noise) 
#endif
	pinMode(A0,INPUT);
	pinMode(A1,INPUT);
	pinMode(A2,INPUT);
	pinMode(D0, INPUT_PULLUP);
	pinMode(D1, INPUT_PULLUP);
	Wire.setTimeout(100);
	Wire.setSDA(D0);
	Wire.setSCL(D1);
	Wire.setClock(100000);
	Wire.begin();

	analogReadResolution(11);
}


uint16_t hue;



//bool reserved_addr(uint8_t addr) {
//		return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
//}

int speed=1;
int i=1;
int LED=0;
int lastTime = micros();
int timed1 = millis();
int timed2 = millis();
int update_cooldown = micros();
int count = 0;
int report = 0;
/*
	========
		Loop
	========
 */
void loop()
{

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
		
		//led.R = 0; led.G = LED; led.B = 0;

	} else if (TinyUSBDevice.suspended())
	{
		//led.r = 0; led.g = 0;	led.b = 0;
		LED = 0;
	}	else {
		LED = 1;
		//led.r = LED; led.g = 0; led.b = 0;
	}
	hue+=25;
	if (hue >= UINT16_MAX)
	hue = 0;
	//if(!TinyUSBDevice.ready() ) { return; }
	//if (millis() - 10 > timed2) {
		timed2 = millis();
	  //hue += 10;  // You can adjust the increment value to control the speed of color change

		//auto led = RGBtoHue(hue, (float)(LED / 31.0f));
		for (int p = 0; p < 3; p++) {
			uint16_t _hue = hue + (p * 2 * 65536) / 3;
			led.R = 0*map_clamped<float>(LED,0,31,0,1);
			led.G = 255*map_clamped<float>(LED,0,31,0,1);
			led.B = 0*map_clamped<float>(LED,0,31,0,1);
			uint32_t _pixel = led.raw;
			//uint32_t _pixel = rainbow(_hue, 255, 255*map_clamped<float>(31,0,31,0,1), true);
			auto buffsize = sizeof(command->command_type+command->id);
			buffsize += sizeof(_pixel);
			command->command_type = SET_LED;
			command->id = p;
			command->data = (uint32_t*)_pixel;

			Wire.beginTransmission(0x21);
			Wire.write((byte *)i2cBuff, buffsize);
			Wire.endTransmission();
			//delay(100);
		}
	//}


	// Wake up host if we are in suspend mode
	// and REMOTE_WAKEUP feature is enabled by host
	//if (TinyUSBDevice.suspended() && BOOTSEL)
	//{
	//	TinyUSBDevice.remoteWakeup();
	//}
	

	jr[0].rx = jr[1].rx = map_clamped<uint16_t>(analogRead(A0), 200, 1800, 2047, 0);


	//if (micros() - lastTime > 5000)
	//{
	//	lastTime = micros();
	//	if (LED >= 31 || LED <= 0)
	//		speed = -speed;
	//	LED += speed;
	//}	
	//if (i >= 2047 || i <= 0)
		//speed = -speed;
		//i += speed;
		//jr1.x = analogRead(A0);
		//jr1.y = analogRead(A1);
		//jr1.z = analogRead(A2);
		//jr1.rx = i;
		//jr1.ry = i;
		//jr1.rz = i;
		//jr1.slider = i;
		//jr1.dial = i;
		//jr1.hat1 = i%8;
		//jr1.hat2 = i%8;
		//jr1.buttons = i;
		if (micros() - 2000 > update_cooldown) {
		

			update_cooldown = micros();
			if ( jr[report].x != jr_old[report].x      ||
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
					jr_old[report] = jr[report];
				}
				if (TinyUSBDevice.suspended()) {
					TinyUSBDevice.remoteWakeup();
				}
			}
			report++;
			if (report > JS_COUNT)
			report = 0;
		}

	if (millis() - timed1 > 1000)
	{
		timed1 = millis();
		i2cScanner();
		count++;
		if (count >4 )
		 count = 0;
		 if (count > 3) {
			Serial.println("\n\rRequesting data from 0x21");
			command->command_type = GET_LED;
			command->id = 0;
			size_t size = sizeof(command->command_type+command->id); // only sending command and id (led)

			Wire.beginTransmission(0x21);
			Wire.write((byte*)i2cBuff, size);
			Wire.endTransmission();

			delay(100);

			size += sizeof(uint32_t); //add RGB(W) value from WS2812 to the request

			Wire.requestFrom(0x21,size);
			//i2cBuffSize = i2cBuffSize - Wire.readBytes((byte*)i2cBuff, size+sizeof(uint32_t));
			Serial.println("After I2C RequestFrom(0x21)\n\r");
		 }
	}
}



#define X52_BUSY_WAIT 0
#define X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION 1
#include <x52_hotas.h>
#define MAX_UPDATES_PER_SECOND 300

// TODO: Choose your favorite digital pins on your board.
x52::pro::JoystickClient<D20, D19, D21, D18> joystick_client;
//x52::pro::JoystickConfig cfg;
//x52::pro::JoystickState state;



void serial_log_joystick_state(const x52::pro::JoystickState& state);

void setup1()
{
	joystick_client.Setup();

#if MAX_UPDATES_PER_SECOND
	static x52::util::RateLimiter<MAX_UPDATES_PER_SECOND, MAX_UPDATES_PER_SECOND> rate_limiter;
#endif
}

void loop1()
{

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

	//if(TinyUSBDevice.ready()) {
	//	if (cfg.led_brightness != x52::pro::JoystickConfig::MAX_LED_BRIGHTNESS / 2)	{
			//cfg.led_brightness = x52::pro::JoystickConfig::MAX_LED_BRIGHTNESS / 2;
	//	}
	//} else if (TinyUSBDevice.mounted())
	//{
	//	if (cfg.led_brightness != 0) {
	//		cfg.led_brightness = 0;
	//	}
	//} else {
	//	if (cfg.led_brightness != 0) {
	//		cfg.led_brightness = 0;
	//	}
	//}


	// TODO: set other JoystickConfig values if you want

	auto timeout_micros = joystick_client.PollJoystickState(state, cfg);
	if (timeout_micros) {
		X52DebugPrintln("PollJoystickState failed");
		delayMicroseconds(timeout_micros);
		return;
	}
	bool empty = false;

	jr[0].x = map_clamped<uint16_t>(state.x, 0,1023,0,2046);
	jr[0].y = map_clamped<uint16_t>(state.y, 0,1023,0,2046);
	jr[0].z = map_clamped<uint16_t>(state.z, 0,1023,0,2046);

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
	bitWrite(jr[0].buttons, 0, state.trigger_stage_1);
	bitWrite(jr[0].buttons, 1, state.button_fire);
	bitWrite(jr[0].buttons, 2, state.button_a);
	bitWrite(jr[0].buttons, 3, state.button_b);
	bitWrite(jr[0].buttons, 4, state.button_c);
	bitWrite(jr[0].buttons, 5, state.pinkie_switch);
	bitWrite(jr[0].buttons, 6, empty);
	bitWrite(jr[0].buttons, 7, empty);
	bitWrite(jr[0].buttons, 8, state.button_t1);
	bitWrite(jr[0].buttons, 9, state.button_t2);
	bitWrite(jr[0].buttons, 10, state.button_t3);
	bitWrite(jr[0].buttons, 11, state.button_t4);
	bitWrite(jr[0].buttons, 12, state.button_t5);
	bitWrite(jr[0].buttons, 13, state.button_t6);
	bitWrite(jr[0].buttons, 14, state.trigger_stage_2);
	bitWrite(jr[0].buttons, 15, empty);
	bitWrite(jr[0].buttons, 16, empty);
	bitWrite(jr[0].buttons, 17, empty);
	bitWrite(jr[0].buttons, 18, empty);
	bitWrite(jr[0].buttons, 19, p2u);
	bitWrite(jr[0].buttons, 20, p2r);
	bitWrite(jr[0].buttons, 21, p2d);
	bitWrite(jr[0].buttons, 22, p2l);
	bitWrite(jr[0].buttons, 23, empty);
	bitWrite(jr[0].buttons, 24, empty);
	bitWrite(jr[0].buttons, 25, empty);
	bitWrite(jr[0].buttons, 26, empty);
	bitWrite(jr[0].buttons, 27, M1);
	bitWrite(jr[0].buttons, 28, M2);
	bitWrite(jr[0].buttons, 29, M3);
	bitWrite(jr[0].buttons, 30, empty);
	bitWrite(jr[0].buttons, 31, empty);
	//serial_log_joystick_state(state);

#if MAX_UPDATES_PER_SECOND
	}
#endif
	//PixelPIO.neoPixelClear(true);
}

void serial_log_joystick_state(const x52::pro::JoystickState& state) {
	// DebugPrinting after every 100th call
	static int counter = 0;
	if (counter++ % 100 != 0)
		return;

	X52DebugPrint("x:");
	X52DebugPrint(state.x);
	X52DebugPrint(" y:");
	X52DebugPrint(state.y);
	X52DebugPrint(" z:");
	X52DebugPrint(state.z);

	X52DebugPrint(" pov1:");
	if (state.pov_1 & x52::Up)
		X52DebugPrint("Up");
	if (state.pov_1 & x52::Down)
		X52DebugPrint("Down");
	if (state.pov_1 & x52::Left)
		X52DebugPrint("Left");
	if (state.pov_1 & x52::Right)
		X52DebugPrint("Right");

	X52DebugPrint(" pov2:");
	if (state.pov_2 & x52::Up)
		X52DebugPrint("Up");
	if (state.pov_2 & x52::Down)
		X52DebugPrint("Down");
	if (state.pov_2 & x52::Left)
		X52DebugPrint("Left");
	if (state.pov_2 & x52::Right)
		X52DebugPrint("Right");

	X52DebugPrint(" mode:");
	switch (state.mode) {
		case x52::ModeUndefined: X52DebugPrint("Undefined"); break;
		case x52::Mode1: X52DebugPrint("Mode1"); break;
		case x52::Mode2: X52DebugPrint("Mode2"); break;
		case x52::Mode3: X52DebugPrint("Mode3"); break;
	}

	X52DebugPrint("\ntrigger_stage_1:");
	X52DebugPrint(state.trigger_stage_1);
	X52DebugPrint(" trigger_stage_2:");
	X52DebugPrint(state.trigger_stage_2);
	X52DebugPrint(" pinkie:");
	X52DebugPrint(state.pinkie_switch);
	X52DebugPrint(" fire:");
	X52DebugPrint(state.button_fire);

	X52DebugPrint("\nA:");
	X52DebugPrint(state.button_a);
	X52DebugPrint(" B:");
	X52DebugPrint(state.button_b);
	X52DebugPrint(" C:");
	X52DebugPrint(state.button_c);

	X52DebugPrint(" T1:");
	X52DebugPrint(state.button_t1);
	X52DebugPrint(" T2:");
	X52DebugPrint(state.button_t2);
	X52DebugPrint(" T3:");
	X52DebugPrint(state.button_t3);
	X52DebugPrint(" T4:");
	X52DebugPrint(state.button_t4);
	X52DebugPrint(" T5:");
	X52DebugPrint(state.button_t5);
	X52DebugPrint(" T6:");
	X52DebugPrint(state.button_t6);

	X52DebugPrint("\n");
}

