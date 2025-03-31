
// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define MAX_USB_PACKET_SIZE 64
#define MAX_HID_DESCRIPTOR_SIZE 4096

//#define VID 0x1209
//#define PID 0xedcc
#define VID 0x2e8a
#define PID 0x0003
#define USB_BCD   0x0200
#define USB_LANGUAGE 0x0409 // default is English
#define USB_MANUFACTURER "Raspberry Pi"
#define USB_PRODUCT "Pico W"

// thechnically there is no ID cap since its limited to the descriptor size but... for ease of use we set it
#define MAX_REPORT_ID 31
#define MAX_OUTPUTS 256

//#define PICO_DEFAULT_LED_PIN 25

#include <stdint.h>
#include <string>
#include <stdbool.h>
#include <stdlib.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>

#ifdef RASPBERRYPI_PICO_W 
#include "pico/cyw43_arch.h"
#endif

#include "hardware/spi.h"
//#include "hardware/i2c.h"
//#include "hardware/pio.h"
//#include "hardware/timer.h"
//#include "hardware/clocks.h"
#include "lib/TinyUSB/hw/bsp/board_api.h"

#include "Arduino-wrapper.h"

//==== USB ====
#include "tusb_config.h"
#include "tusb.h"

//==== HID ====
//#include "hid.h" //still need to strip it
//#include "hid_minimal_report.h"
//#include "HID_descriptor.h"
//#include "usb_descriptors.h"


//=============

#include "lib/X52-HOTAS/src/x52_pro.h"
#include "lib/X52-HOTAS/src/x52_util.h"


//==== SD CARD STUFF ====
#include "hw_config.h"
#include "f_util.h"
#include "ff.h"
#include "diskio.h"
//#include "iniConfig-old.h"
//#include "yaml.hpp"

#include "main.h"

//#define LED_PIN 25

int main()
{
  board_init();

  // init device stack on configured roothub port
  tusb_rhport_init_t dev_init = {
    .role = TUSB_ROLE_DEVICE,
    .speed = TUSB_SPEED_AUTO
  };

  // init device stack on configured roothub port
  tusb_init(BOARD_TUD_RHPORT, &dev_init);
//  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  stdio_init_all();

#if defined( CYW43_INCLUDED_CYW43_H)
  // For Pico W devices we need to initialise the driver etc
  auto rc = cyw43_arch_init();
  while (!cyw43_is_initialized(&cyw43_state)) {__nop();}// wait
  hard_assert(rc == PICO_OK);
#endif
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    //return PICO_OK;
#endif
  // SPI initialisation. This example will use SPI at 1MHz.
//  spi_init(spi0,1000 * 1000);
//  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);//16
//  gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);//17
//  gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);//18
//  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);//19

    
  // Chip select is active-low, so we'll initialise it to a driven-high state
//  gpio_set_dir(PIN_CS, GPIO_OUT);
//  gpio_put(PIN_CS, 1);
  // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

  // I2C Initialisation. Using it at 400Khz.
//  i2c_init(I2C_PORT, 400*1000);
    
//  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
//  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
//  gpio_pull_up(I2C_SDA);
//  gpio_pull_up(I2C_SCL);
  // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

  //mount_sdcard();
  //pSD = sd_get_by_num(0);
//  fr = f_mount(&fs, "", 1);
  
//    if (FR_OK != fr) {
//    panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
//  }

//  delay(10);
//  get_config("config/settings.ini");

//  setup_hid_descriptor();
//  setup_usb_configurator();
//  usb_hid.begin();

  while (1) {
//    puts_raw(ini.config().c_str());
    tud_task();
    cdc_task();
    led_blinking_task();
  
//    readyToUpdate[1]=true;
//    if (USBDevice.ready()) {
//      if (readyToUpdate[report] ){
//        reports[1][1] = random();
//        usb_hid.sendReport(report, (void *)reports[report], total_bits[1]/8);
//        memcpy(old_reports[report],(void *)reports[report],total_bits[1]/8);
//        memset((void *)reports[report], 0, sizeof(reports[report]));
//        readyToUpdate[report] = false;
//      }
//      report++;
//      if (report >= IdCount)
//        report = 1;
//    }
//    led_blinking_task();
//  }

//    hid_task();
//        printf("Hello, world!\n");
//        sleep_ms(1000);
    }
}

#if false

void setup_hid_descriptor() {
  uint16_t maxBuffSize = MAX_HID_DESCRIPTOR_SIZE;
  if (enableMouse)
    maxBuffSize -= 79;

  if (enableKeyboard)
    maxBuffSize -= 67;

  // reset the connection
  memset(inputs_id, 0, sizeof(inputs_id));
  memset(usb_hid_descriptor, 0, sizeof(usb_hid_descriptor));
  memset(button_start,0,sizeof(button_start));
  memset(hat_start,0,sizeof(hat_start));
  memset(axis_start,0,sizeof(axis_start));
  memset(total_bits,0,sizeof(total_bits));
  usb_hid_descriptor_length=0;
  IdCount=0;
  
  //this buffer will be cleared and freed at the end since its a temp buffer
  uint8_t* t_buffer = (uint8_t*)malloc(150);
  memset(t_buffer,0,150);
  size_t t_buffer_size;

  /* 
    Calculate how many "devices" we will need to emulate
    Calcucate how many buttons per "device"
    Calculate how many hats per "device"
    Calculate how many axis per "device"
  */
 
  for (uint16_t i=ButtonCount, j=1; i>0; i-=MIN(device_max_button_count,i), j++) {
    inputs_id[j].buttons = i>device_max_button_count ? device_max_button_count:i;
    IdCount = j>IdCount ? j:IdCount;
  }
  for (uint16_t i=HatCount, j=1; i>0; i-=MIN(device_max_hat_count, i), j++) {   
    inputs_id[j].hats = i>device_max_hat_count ? device_max_hat_count:i;
    IdCount = j>IdCount ? j:IdCount;
  }
  for (uint8_t i=AxisCount, j=1; i>0;i-=MIN(device_max_axis_count,i), j++) {
    inputs_id[j].axis = i>device_max_axis_count ? device_max_axis_count:i;
    IdCount = j>IdCount ? j:IdCount;
  }
  for (uint16_t id = 1; id<=IdCount; id++) {
    t_buffer_size = 0;
    makeJoystickDescriptor(id, AxisResolution, inputs_id[id].axis, inputs_id[id].hats, inputs_id[id].buttons, t_buffer, &t_buffer_size);
    if (usb_hid_descriptor_length + t_buffer_size < maxBuffSize) {
      memmove(usb_hid_descriptor+usb_hid_descriptor_length, t_buffer, t_buffer_size);
      usb_hid_descriptor_length += t_buffer_size;
    }
    memset(t_buffer, 0, 150);
//    makeJoystickDescriptor(id, AxisResolution, inputs_id[id].axis, inputs_id[id].hats, inputs_id[id].buttons, usb_report, &usb_report_size);
  }
  if (enableKeyboard) {
    t_buffer_size = 0;
    makeKeyboardDescriptor(IdCount+1,t_buffer, &t_buffer_size);
    if (usb_hid_descriptor_length + t_buffer_size < MAX_HID_DESCRIPTOR_SIZE) {
      memmove(usb_hid_descriptor+usb_hid_descriptor_length,t_buffer,t_buffer_size);
      usb_hid_descriptor_length += t_buffer_size;
      memset(t_buffer,0,150);
    }
  }
  if (enableMouse) {
    t_buffer_size = 0;
    makeMouseDescriptor(IdCount+2,t_buffer, &t_buffer_size);
    if (usb_hid_descriptor_length + t_buffer_size < MAX_HID_DESCRIPTOR_SIZE) {
      memmove(usb_hid_descriptor+usb_hid_descriptor_length,t_buffer,t_buffer_size);
      usb_hid_descriptor_length += t_buffer_size;
      memset(t_buffer,0,150);
    }
  }
  free(t_buffer);
}
#endif

void setup_usb_configurator() {

//  USBDevice.detach();
//  USBDevice.clearConfiguration();

//  USBDevice.setID(VID,PID);
//  USBDevice.setManufacturerDescriptor("Raspberry Pi");
//  USBDevice.setProductDescriptor(DeviceName.c_str());
//  USBDevice.addStringDescriptor("mtp");
//  USBDevice.addStringDescriptor("MTP");

//  if (enableCdc) {
//    if (Serial.isValid())
//      USBDevice.addInterface(Serial);
//    else
//      Serial.begin(0);
//  }
//  if (IdCount > 0) {
  //  char name;
  //  name = DeviceName.concat("Mouse/Keyboard");
//    usb_hid.setPollInterval(1);
//    usb_hid.setBootProtocol(HID_ITF_PROTOCOL_NONE);
//    usb_hid.setReportDescriptor(usb_hid_descriptor, usb_hid_descriptor_length);
//    usb_hid.setReportCallback(get_report_callback, set_report_callback);
//  ///  usb_hid.setStringDescriptor(&name);
//    if (usb_hid.isValid())
//      USBDevice.addInterface(usb_hid);
//    else
//      usb_hid.begin();
//  }
//  if (enableMsc) {
//    usb_msc.setID("test", "card", "0.0");
//    usb_msc.setReadWriteCallback(msc_read_cb,msc_write_cb,msc_flush_cb);

    // Still initialize MSC but tell usb stack that MSC is not ready to read/write
    // If we don't initialize, board will be enumerated as CDC only
//    usb_msc.setUnitReady(false);
//    usb_msc.setCapacity(1024*4096, 512);

//    usb_msc.begin();
    //TODO
//  }
//  if(enableMtp){
//    usb_mtp.setStringDescriptor("MTP");
//    USBDevice.addInterface(usb_mtp);
//  }
//  USBDevice.attach();
}


//void mount_sdcard() {
// fr = f_mount(&fs, "", 1);
//  
//  if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
//}

// im sure there is a better way but this works...
//void get_config(const char *path) {
//
//  f_open(&fil, path, FA_READ);
//  uint br; // dont really care
//  assert(&fil);
//
//  str_config = "";
//  str_config.reserve(f_size(&fil));
//  uint8_t buf[256];
//
//  while (!f_eof(&fil))
//  {
//    f_read(&fil, buf, sizeof(buf), &br);
//    str_config.append((char *)buf, br);
//  }
//  yaml.parse(str_config);
////  yaml.getChildNodeValue()
//  ini.open(&fil,path,FA_READ);
//  ini.readFile(&fil);
//  DeviceName         = ini.readString("hid_report","DeviceName");
//  enableMouse        = ini.readBool("hid report", "enableMouse");
//  enableKeyboard     = ini.readBool("hid report", "enableKeyboard");
//  enableMsc          = ini.readBool("hid report", "enableMsc");
//  hid_usage_page_val = ini.readInt("hid report", "usagepage");  hid_usage_val      = ini.readInt("hid report", "usage");
//  ButtonCount        = ini.readInt("hid report", "buttoncount");
//  HatCount           = ini.readInt("hid report", "hatcount");
//  AxisCount          = ini.readInt("hid report", "axiscount");
//  AxisResolution     = ini.readInt("hid report", "axisresolution");
//  ADCResolution      = ini.readInt("hid report", "adcresolution");
//}






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









//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}




//--------------------------------------------------------------------+
// USB MSC
//--------------------------------------------------------------------+

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
//int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize) {
//  (void) bufsize;
//  return -1;//return card.readBlock(lba, (uint8_t*) buffer) ? 512 : -1;
//}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
//int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
//  (void) bufsize;
//  return -1;//return card.writeBlock(lba, buffer) ? 512 : -1;
//}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
//void msc_flush_cb (void) {
  // nothing to do
//}


// //--------------------------------------------------------------------+
// // USB CDC
// //--------------------------------------------------------------------+
#if CFG_TUD_CDC
void cdc_task(void) {
  // connected() check for DTR bit
  // Most but not all terminal client set this when making connection
  // if ( tud_cdc_connected() )
  {
    // connected and there are data available
    if (tud_cdc_available()) {
      // read data
      char buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));
      (void) count;
      // Echo back
      // Note: Skip echo by commenting out write() and write_flush()
      // for throughput test e.g
      //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
      tud_cdc_write(buf, count);
      tud_cdc_write_flush();
    }
  }
}

// // Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void) itf;
  (void) rts;

  // TODO set some indicator
  if (dtr) {
    // Terminal connected
  } else {
    // Terminal disconnected
  }
}

// // Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
  (void) itf;
}

#endif

// // //--------------------------------------------------------------------+
// // // USB HID
// // //--------------------------------------------------------------------+
#if CFG_TUD_HID

/*
  =============
    Callbacks  
  =============
*/

//uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
//{
//  (void) instance;
//  return usb_hid_descriptor;
//}





// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  memset(buffer,0,reqlen);
  printf("get_report type: ");
  printf((int)report_type + "\n");
  printf("get_report id: ");
  printf((int)report_id+ "\n");
  switch (report_type)
  {
    case (hid_report_type_t)HID_REPORT_TYPE_INVALID:
    case (hid_report_type_t)HID_REPORT_TYPE_OUTPUT:
    break;

    case (hid_report_type_t)HID_REPORT_TYPE_INPUT:
    case (hid_report_type_t)HID_REPORT_TYPE_FEATURE:
    if (report_id < 1) {
      buffer[0] = IdCount;
    } else if (report_id > 0 && report_id <= MAX_REPORT_ID) {
      buffer[0] = inputs_id[report_id].buttons;
      buffer[1] = inputs_id[report_id].hats;  
      buffer[2] = inputs_id[report_id].axis;
      buffer[3] = AxisResolution;
    } else {break;}
    
    return reqlen;
  }
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  printf("set_report type: ");
  printf((int)report_type + "\n");
  printf("set_report id: ");
  printf(report_id + "\n");
 
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


// // // THIS WILL BE REWRITTEN
// // static void send_hid_report(uint8_t report_id, uint32_t btn)
// // {
// //   // skip if hid is not ready yet
// //   if ( !tud_hid_ready() ) return;

// //   switch(report_id)
// //   {
// //     case REPORT_ID_KEYBOARD:
// //     {
// //       // use to avoid send multiple consecutive zero report for keyboard
// //       static bool has_keyboard_key = false;

// //       if ( btn )
// //       {
// //         uint8_t keycode[6] = { 0 };
// //         keycode[0] = HID_KEY_A;

// //         tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
// //         has_keyboard_key = true;
// //       }else
// //       {
// //         // send empty key report if previously has key pressed
// //         if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
// //         has_keyboard_key = false;
// //       }
// //     }
// //     break;

// //     case REPORT_ID_MOUSE:
// //     {
// //       int8_t const delta = 5;

// //       // no button, right + down, no scroll, no pan
// //       tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
// //     }
// //     break;

// //     case 99:
// //     {
// //       // use to avoid send multiple consecutive zero report
// //       static bool has_consumer_key = false;

// //       if ( btn )
// //       {
// //         // volume down
// //         uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
// //         tud_hid_report(NULL, &volume_down, 2);
// //         has_consumer_key = true;
// //       }else
// //       {
// //         // send empty key report (release key) if previously has key pressed
// //         uint16_t empty_key = 0;
// //         if (has_consumer_key) tud_hid_report(NULL, &empty_key, 2);
// //         has_consumer_key = false;
// //       }
// //     }
// //     break;

// //     case 100:
// //     {
// //       // use to avoid send multiple consecutive zero report for keyboard
// //       static bool has_gamepad_key = false;

// //       hid_gamepad_report_t __report =
// //       {
// //         .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
// //         .hat = 0, .buttons = 0
// //       };

// //       if ( btn )
// //       {
// //         __report.hat = GAMEPAD_HAT_UP;
// //         __report.buttons = GAMEPAD_BUTTON_A;
// //         tud_hid_report(NULL, &__report, sizeof(__report));

// //         has_gamepad_key = true;
// //       }else
// //       {
// //         __report.hat = GAMEPAD_HAT_CENTERED;
// //         __report.buttons = 0;
// //         if (has_gamepad_key) tud_hid_report(NULL, &__report, sizeof(__report));
// //         has_gamepad_key = false;
// //       }
// //     }
// //     break;

// //     default: break;
// //   }
// // }

// // Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// // tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
//   // Poll every 10ms
//   const uint32_t interval_ms = 10;
//   static uint32_t start_ms = 0;

//   if ( board_millis() - start_ms < interval_ms) return; // not enough time
//   start_ms += interval_ms;

//   uint32_t const btn = board_button_read();

//   // Remote wakeup
//   if ( tud_suspended() && btn )
//   {
//     // Wake up host if we are in suspend mode
//     // and REMOTE_WAKEUP feature is enabled by host
//     tud_remote_wakeup();
//   }else
//   {
//     // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
//     send_hid_report(REPORT_ID_KEYBOARD, btn);
//   }
}

// // Invoked when sent REPORT successfully to host
// // Application can use this to send the next report
// // Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

//  uint8_t next_report_id = report[0] + 1u;

//  if (next_report_id < REPORT_ID_COUNT)
//  {
//    send_hid_report(next_report_id, board_button_read());
//  }
}

// // Invoked when received GET_REPORT control request
// // Application must fill buffer report's content and return its length.
// // Return zero will cause the stack to STALL request
//uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
//{
//  // TODO not Implemented
//  (void) instance;
//  (void) report_id;
//  (void) report_type;
//  (void) buffer;
//  (void) reqlen;
//  return 0;
//}

// // Invoked when received SET_REPORT control request or
// // received data on OUT endpoint ( Report ID = 0, Type = 0 )
//void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
//{
//   (void) instance;

//   if (report_type == HID_REPORT_TYPE_OUTPUT)
//   {
//     // Set keyboard LED e.g Capslock, Numlock etc...
//     if (report_id == REPORT_ID_KEYBOARD)
//     {
//       // bufsize should be (at least) 1
//       if ( bufsize < 1 ) return;

//       uint8_t const kbd_leds = buffer[0];

//       if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
//       {
//         // Capslock On: disable blink, turn led on
//         blink_interval_ms = 0;
//         board_led_write(true);
//       }else
//       {
//         // Caplocks Off: back to normal blink
//         board_led_write(false);
//         blink_interval_ms = BLINK_MOUNTED;
//       }
//     }
//   }
//}
#endif
//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void) {
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

#if defined(PICO_DEFAULT_LED_PIN)
  // Just set the GPIO on or off
  gpio_put(PICO_DEFAULT_LED_PIN, led_state);
#elif defined(CYW43_WL_GPIO_LED_PIN)
  // Ask the wifi "driver" to set the GPIO on or off
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
#endif//  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

//void led_blinking_task(void)
//{
//  static uint32_t start_ms = 0;
//  static bool led_state = false;
//  for (;;) {
//    // blink is disabled
//    if (!blink_interval_ms) return;
//
//    // Blink every interval ms
////    if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
//    if ( millis() - start_ms < blink_interval_ms) return; // not enough time
//    start_ms += blink_interval_ms;
//
////    board_led_write(led_state);
//#if defined(PICO_DEFAULT_LED_PIN)
//    // Just set the GPIO on or off
//    gpio_put(PICO_DEFAULT_LED_PIN, led_state);
//#elif defined(CYW43_WL_GPIO_LED_PIN)
//    // Ask the wifi "driver" to set the GPIO on or off
//    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
//#endif
//    led_state = 1 - led_state; // toggle
//  }
//}
//