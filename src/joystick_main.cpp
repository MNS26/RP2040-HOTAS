/*== JOYSTICK ==*/
#ifdef USE_TINYUSB
#include <Arduino.h>
#include "hid.hpp"
#include "Adafruit_TinyUSB.h"
#include "HID_descriptors.hpp"
#include "HID_rep.hpp"
#include "pio_usb.h"
#endif
#include "btn_axis.hpp"

/*== LED ==*/
#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 1
#define DATA_PIN 16

uint8_t const desc_hid_report[] = 
{
  TUD_HID_REPORT_DESC_AXIS(HID_REPORT_ID(1)),
  TUD_HID_REPORT_DESC_GENERIC_INOUT(64)
};


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
int interval = 2;
Adafruit_USBD_HID hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, interval, true);

// Create an instance of the joystick report data struct
JoystickAxisReport joystickReport = {
  //.XYZ = 0,
  //.RxRyRz = 0,
  //.VxVyVz = 0,
  //.SDW = 0,
  //.buttons1 = 0,
	//.buttons2 = 0,
	//.buttons3 = 0,
  //.hat = 0,
  //.thumbstickYX = 0,
};
JoystickButtonsReport joystickButtonReport;
int timeout,timer,speed=1,i;

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // not used in this example
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // This example doesn't use multiple report and report ID
  (void) report_id;
  (void) report_type;

  // echo back anything we received from host
  //hid.sendReport(0, buffer, bufsize);
}


/*
	=========
	  Setup
	=========
 */
void setup()
{
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as
  // - mbed rp2040
  TinyUSB_Device_Init(0);
#endif
  hid.setReportCallback(get_report_callback, set_report_callback);
	hid.begin();
  SerialTinyUSB.begin(9600);

	// start the USB device
 	USBDevice.attach();

//	strip.begin();

	// wait until device mounted
	while (!TinyUSBDevice.mounted())
	{	}
	int count = sizeof(joystickButtonReport) / sizeof(bool);  // 10
}
bool invert(bool input){
	if(input)
	return false;
	return true;
}

/*
	========
		Loop
	========
 */
int button_pres;
void loop()
{
  // Remote wakeup
  if ( TinyUSBDevice.suspended() )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }
	if(!TinyUSBDevice.ready())
	{
		return;
	}
	else
	{
	}
	delay(1);
  i+=speed;
	if(i < 1 || i > 1022)
		speed=-speed;

	setAxisValue(&joystickReport, X,      i);
	setAxisValue(&joystickReport, Y,      i);
	setAxisValue(&joystickReport, Z,      i);
	setAxisValue(&joystickReport, Rx,     i);
	setAxisValue(&joystickReport, Ry,     i);
	setAxisValue(&joystickReport, Rz,     i);
	//setAxisValue(&joystickReport, Vx,     i);
	//setAxisValue(&joystickReport, Vy,     0);
	//setAxisValue(&joystickReport, Vz,     0);
	setAxisValue(&joystickReport, Slider, i);
	setAxisValue(&joystickReport, Dial,   i);
	setAxisValue(&joystickReport, Wheel,  i);
	if (i<14)
	setAxisValue(&joystickReport, MouseX, i);
  setAxisValue(&joystickReport, MouseY, i);
	for (int i = 0; i < 80; i++)
  {
  }

	//setButtonState(&joystickReport,joystickReport.buttons1,random(0,UINT16_MAX));
	hid.sendReport(1, &joystickReport, sizeof(joystickReport));
//	hid.sendReport(2, &joystickButtonReport, sizeof(joystickButtonReport));
}


