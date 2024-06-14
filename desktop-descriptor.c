#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_HID_DESCRIPTOR_SIZE 1024

#include "hid_minimal.h"
#include "include/HID_descriptor.h"

#define TU_ATTR_PACKED __attribute__((packed))
#define TU_BIT(n) (1<<n)

int main(int argc, char **argv) {
  unsigned int size;
  uint8_t *hid_descriptor = makeDescriptor(8, 8, 2, 8, &size);

  for (int r = 1; r<MAX_REPORT_ID; r++) {
    for (int i=0; i<output_index; i++) {
      printf("output %d page 0x%x usage 0x%x starts at bit %d and is %d bits long\n", i, outputs[r][i].usage_page, outputs[r][i].usage, outputs[r][i].offset, outputs[r][i].bits);
    }
  }
}
