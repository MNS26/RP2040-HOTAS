#include "hid.h"

uint8_t *makeDescriptor(uint8_t bitsPerAxis, uint8_t axisCount,uint8_t hatCount, uint8_t buttonCount, unsigned int *bufferSize) {
  uint8_t *buffer = (uint8_t*)malloc(MAX_HID_DESCRIPTOR_SIZE);
  uint8_t *p = buffer;

  bits = 0;
  sp = 0;

  hid_usage_page(&p, HID_USAGE_PAGE_DESKTOP );
  hid_usage(&p, HID_USAGE_DESKTOP_JOYSTICK );
  hid_collection(&p, HID_COLLECTION_APPLICATION );
    output_index = 0;
    hid_report_id(&p, 1);
    hid_usage_page(&p, HID_USAGE_PAGE_DESKTOP );
    hid_usage(&p ,HID_USAGE_DESKTOP_POINTER );
    hid_collection(&p, HID_COLLECTION_PHYSICAL );
      hid_report_count(&p, 1);
      hid_logical_min(&p, 0);
      hid_physical_min(&p, -32768);
      hid_physical_max(&p, 32767);
      for (uint8_t i = 0; i < axisCount; i++) {
        hid_usage(&p, HID_USAGE_DESKTOP_X+i);
        hid_logical_max(&p, (1<<bitsPerAxis)-1);
        hid_report_size(&p, bitsPerAxis);
        hid_input(&p, HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION);
        if ((8 - (bitsPerAxis % 8)) % 8) {
          hid_report_count(&p, 1);
          hid_report_size(&p, (8 - (bitsPerAxis % 8)) % 8);
          hid_input(&p, HID_CONSTANT);
        }
      }
      hid_push(&p);
        for (uint8_t i = 0; i < hatCount; i++) {
          hid_usage_page(&p, HID_USAGE_DESKTOP_HAT_SWITCH);
        }
        hid_logical_min(&p, 0);
        hid_logical_max(&p, 8);
        hid_physical_min(&p, 0);
        hid_physical_max(&p, 315);
        hid_report_count(&p, hatCount);
        hid_report_size(&p, 4);
        hid_input(&p, HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_PREFERRED_STATE|HID_NO_NULL_POSITION);

        if ((8 - (hatCount * 4 % 8)) % 8) {
          hid_report_count(&p, 1);
          hid_report_size(&p, (8 - (hatCount * 4 % 8)) % 8);
          hid_input(&p, HID_CONSTANT);
        }
      hid_pop(&p);
      hid_usage_page(&p, HID_USAGE_PAGE_BUTTON);
        hid_usage_min(&p, 1);
        hid_usage_max(&p, buttonCount);
        hid_logical_min(&p, 0);
        hid_logical_max(&p, 1);
        hid_report_count(&p, buttonCount);
        hid_report_size(&p, 1);
        hid_input(&p, HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION);

        if( (8 - (buttonCount % 8)) % 8) {
          hid_report_count(&p, 1);
          hid_report_size(&p, (8 - (buttonCount % 8)) % 8);
          hid_input(&p, HID_CONSTANT);
        }
    hid_collection_end(&p);
  hid_collection_end(&p);

  *bufferSize = p - buffer;
  return buffer;
}
