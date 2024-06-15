#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_HID_DESCRIPTOR_SIZE 1024

#define TU_ATTR_PACKED __attribute__((packed))
#define TU_BIT(n) (1<<n)

#include "include/hid_minimal.h"
#include "include/HID_descriptor.h"

int min(int x, int y) {return y^((x^y)&-(x<y));}
 
int max(int x, int y) {return x^((x^y)&-(x<y));}

int main(int argc, char **argv) {
  uint8_t reportIDS   = 3;
  uint8_t axisBits    = 8;
  uint8_t axisCount   = 1;
  uint8_t hatCount    = 2;
  uint8_t buttonCount = 8;
  unsigned int size;
  uint8_t *hid_descriptor;

  printf("report ids %02d max reports %02d\n\n",
    reportIDS,
    MAX_REPORT_ID);
  
  printf("axis bits   %02d\nnum axis    %02d\nnum hats    %02d\nnum buttons %02d\n\n",
    axisBits,
    axisCount,
    hatCount,
    buttonCount
  );
  
  hid_descriptor = makeDescriptor(1, axisBits, axisCount, hatCount, buttonCount, &size);

  for (int r = 1; r<=min(reportIDS, MAX_REPORT_ID); r++) {
    printf("report ID: %02d\n",r);
    for (int i=0; i<output_count[r]; i++) {
      printf(
        "output %03d page 0x%02x usage 0x%02x starts at bit %04d and is %02d bits long\n", 
        i,
        outputs[r][i].usage_page,
        outputs[r][i].usage,
        outputs[r][i].offset,
        outputs[r][i].bits
      );
    }
    printf("\n");
  }
}
