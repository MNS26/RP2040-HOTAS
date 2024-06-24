#define DESKTOP_ONLY
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_HID_DESCRIPTOR_SIZE 4096

#define TU_ATTR_PACKED __attribute__((packed))
#define TU_BIT(n) (1<<n)

#include "include/hid_minimal.h"
#include "include/HID_descriptor.h"

typedef struct {
  uint8_t buttons;
  uint8_t hats;
  uint8_t axis;
} input_id;

uint8_t device_max_button_count = 79;
uint8_t device_max_hat_count = 4;
uint8_t device_max_axis_count = 9;

uint32_t AxisResolution = 11;
uint16_t AxisCount = 8;
uint16_t ButtonCount = 65;
uint16_t HatCount = 1;
uint8_t hid_usage_page_val = HID_USAGE_PAGE_DESKTOP;
uint8_t hid_usage_val = HID_USAGE_DESKTOP_JOYSTICK;

uint8_t DeviceCount=0;
uint usb_report_size;
uint8_t usb_report[MAX_HID_DESCRIPTOR_SIZE];

input_id inputs_id[MAX_REPORT_ID]; 
uint16_t axis_start[MAX_REPORT_ID];
uint16_t button_start[MAX_REPORT_ID];
uint16_t hat_start[MAX_REPORT_ID];
uint16_t total_bits[MAX_REPORT_ID];
uint8_t reports[MAX_REPORT_ID][64];
uint8_t old_reports[MAX_REPORT_ID][64];
uint8_t readyToUpdate[MAX_REPORT_ID];
uint16_t largest_bits;


int min(int x, int y) {return y^((x^y)&-(x<y));}

int max(int x, int y) {return x^((x^y)&-(x<y));}

#define MIN(x,y) min(x,y)
#define MAX(x,y) max(x,y)

void set_button(uint8_t *report, int reportid, uint button, uint value) {
  int bit = button_start[reportid] + button;
  int byte = bit / 8;
  int shift = bit % 8;
  report[byte] |= (value & 1) << shift;
}

void set_axis(uint8_t *report, int reportid, int axis, uint32_t value) {
  int remaining_bits = AxisResolution;
  int bitoffset = axis_start[reportid] + (AxisResolution * axis);
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

void set_hat(uint8_t *report, uint8_t reportid, uint8_t hat, uint8_t value) {
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

void setupUSB() {

  /* 
    Calculate how many "devices" we will need to emulate
    Calcucate how many buttons per "device"
    Calculate how many hats per "device"
    Calculate how many axis per "device"
  */
  memset(inputs_id, 0, sizeof(inputs_id));
  memset(usb_report, 0, sizeof(usb_report));
  memset(button_start,0, sizeof(button_start));
  memset(hat_start,0, sizeof(hat_start));
  memset(axis_start,0, sizeof(axis_start));
  usb_report_size=0;
  largest_bits=0;
  DeviceCount=0;

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
}

int main(int argc, char **argv) {
  
  
  printf("axis bits   %02d\nnum axis    %02d\nnum hats    %02d\nnum buttons %02d\n\n",
    AxisResolution,
    AxisCount,
    HatCount,
    ButtonCount
  );
  setupUSB();
  AxisCount = 0;
  setupUSB();
  for (uint8_t i = 0; i <8; i++) {
    set_axis(reports[1],1,i,(1<<AxisResolution)-1);
  }
  for (uint8_t i = 0; i <64; i++) {
    set_button(reports[1],1,i,1);
  }
  for (uint8_t i = 0; i <1; i++) {
    set_hat(reports[1],1,i,0B00001111);
  }
  for (int r = 1; r< MAX_REPORT_ID; r++) {
    printf("report ID: %02d\n",r);
    for (int i=0; i<output_count[r]; i++) {
      printf(
        "output %03d | page 0x%02x | usage 0x%02x | start bit %04d end bit %04d | %02d bits long | input variable %03d", 
        i, outputs[r][i].usage_page, outputs[r][i].usage, outputs[r][i].offset, outputs[r][i].offset+outputs[r][i].bits, outputs[r][i].bits, outputs[r][i].input_variable 
      );

      if (outputs[r][i].input_variable != HID_CONSTANT) {
        if (outputs[r][i].usage >= 0x30 && outputs[r][i].usage <= 0x38) {
        printf(" (axis)");
        } else if (outputs[r][i].usage_page == 0x39) {
          printf(" (hat)");
        } else if (outputs[r][i].usage_page == 0x09) {
          printf(" (button)");
        }
      }
      else {
        printf(" (padding)");
      }
      printf("\n");
    }
    printf("\n");
  }

  FILE *fptr;
  fptr = fopen("descriptor.bin", "w");
  if (fptr) {
    fwrite(usb_report, 1, usb_report_size, fptr);
  }
  fclose(fptr);

//  for (;;) {
//
//  }
}
