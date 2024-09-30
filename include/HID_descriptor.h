#include "hid.h"

void makeJoystickDescriptor(uint8_t reportID, uint8_t bitsPerAxis, uint8_t axisCount,uint8_t hatCount, uint8_t buttonCount, uint8_t *buffer, uint *bufferSize) {
  extern uint16_t axis_start[];
  extern uint16_t hat_start[];
  extern uint16_t button_start[];
  extern uint16_t total_bits[];
  extern uint8_t hid_usage_val;
  extern uint8_t hid_usage_page_val;

  uint8_t *p = buffer+*bufferSize;
  sp=0;
  bits=0;

  hid_usage_page(&p, hid_usage_page_val );
  hid_usage(&p, hid_usage_val );
  hid_collection(&p, HID_COLLECTION_APPLICATION );
#ifdef DESKTOP_ONLY
    output_index = 0;
#endif
    hid_report_id(&p, reportID);
    //hid_usage_page(&p, hid_usage_page_val );
    //hid_usage(&p ,hid_usage_val );
    //hid_collection(&p, HID_COLLECTION_PHYSICAL );

      if (buttonCount) {
        button_start[reportID] = bits;
        hid_usage_page(&p, HID_USAGE_PAGE_BUTTON);
          hid_usage_min(&p, 1);
          hid_usage_max(&p, buttonCount);
          hid_logical_min(&p, 0);
          hid_logical_max(&p, 1);
          hid_report_count(&p, buttonCount);
          hid_report_size(&p, 1);
          hid_input(&p, HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION);

          if (bits % 8) {
            hid_report_count(&p, 1);
            hid_report_size(&p, 8 - (bits % 8));
            hid_input(&p, HID_CONSTANT);
          }
      }
      
      if (hatCount>0 || axisCount>0)
        hid_usage_page(&p, HID_USAGE_PAGE_DESKTOP);

      if (hatCount) {
        hat_start[reportID] = bits;
        for (uint8_t i = 0; i < hatCount; i++) {
          hid_usage(&p, HID_USAGE_DESKTOP_HAT_SWITCH);
        }
        hid_logical_min(&p, 1);
        hid_logical_max(&p, 8);
        hid_physical_min(&p, 0);
        hid_physical_max(&p, 315);
        hid_report_count(&p, hatCount);
        hid_report_size(&p, 4);
        hid_input(&p, HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_PREFERRED_STATE|HID_NO_NULL_POSITION);

        if (bits % 8) {
          hid_report_count(&p, 1);
          hid_report_size(&p, 8 - (bits % 8));
          hid_input(&p, HID_CONSTANT);
        }
      }

      if (axisCount) {
        axis_start[reportID] = bits;
        for (uint8_t i = 0; i < axisCount; i++) {
          hid_usage(&p, HID_USAGE_DESKTOP_X+i);
        }
        hid_logical_min(&p, 0);
        hid_logical_max(&p, (1<<bitsPerAxis)-1);
        hid_physical_min(&p, -32768);
        hid_physical_max(&p, 32767);
        hid_report_count(&p, axisCount);
        hid_report_size(&p, bitsPerAxis);
        hid_input(&p, HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION);

        if (bits % 8) {
          hid_report_count(&p, 1);
          hid_report_size(&p, 8 - (bits % 8));
          hid_input(&p, HID_CONSTANT);
        }
      }

    //hid_collection_end(&p);
  hid_collection_end(&p);
  total_bits[reportID] = bits;
  *bufferSize = p - buffer;
}

uint8_t *_makeJoystickDescriptor(uint8_t reportID, uint8_t bitsPerAxis, uint8_t axisCount,uint8_t hatCount, uint8_t buttonCount,uint *bufferSize) {
  uint8_t *buffer = (uint8_t*)malloc(MAX_HID_DESCRIPTOR_SIZE);
  *bufferSize = 0;
  makeJoystickDescriptor(reportID, bitsPerAxis, axisCount, hatCount, buttonCount, buffer, bufferSize);
  return buffer;
}

void makeKeyboardDescriptor(uint8_t reportID, uint8_t *buffer, uint *bufferSize) {
  extern uint16_t total_bits[];
  uint8_t *p = buffer+*bufferSize;
  sp=0;
  bits=0;
  
  hid_usage_page(&p, HID_USAGE_PAGE_DESKTOP);
  hid_usage(&p, HID_USAGE_DESKTOP_KEYBOARD);
  hid_collection(&p, HID_COLLECTION_APPLICATION);
  
    hid_report_id(&p, reportID);
  
    hid_usage_page(&p, HID_USAGE_PAGE_KEYBOARD);
      hid_usage_min(&p, 224);
      hid_usage_max(&p, 231);
      hid_logical_min(&p, 0);
      hid_logical_max(&p, 1);
      hid_report_count(&p, 8);
      hid_report_size(&p, 1);
      hid_input(&p, HID_DATA | HID_VARIABLE | HID_ABSOLUTE);

      hid_report_count(&p, 1);
      hid_report_size(&p, 8);
      hid_input(&p, HID_CONSTANT);

    hid_usage_page(&p, HID_USAGE_PAGE_LED);
      hid_usage_min(&p, 1);
      hid_usage_max(&p, 5);
      hid_report_count(&p, 5);
      hid_report_size(&p, 1);
      hid_input(&p, HID_DATA | HID_VARIABLE | HID_ABSOLUTE);

      hid_report_count(&p, 1);
      hid_report_size(&p, 3);
      hid_input(&p, HID_CONSTANT);

    hid_usage_page(&p, HID_USAGE_PAGE_KEYBOARD);
      hid_usage_min(&p, 0);
      hid_usage_max(&p, 255);
      hid_logical_min(&p, 0);
      hid_logical_max(&p, 255);
      hid_report_count(&p, 6);
      hid_report_size(&p, 8);
      hid_input(&p, HID_DATA | HID_ARRAY | HID_ABSOLUTE);
    hid_collection_end(&p);

  total_bits[reportID] = bits;
  *bufferSize = p - buffer;

}
void makeMouseDescriptor(uint8_t reportID, uint8_t *buffer, uint *bufferSize) {
  extern uint16_t total_bits[];
  uint8_t *p = buffer+*bufferSize;
  sp=0;
  bits=0;

  hid_usage_page(&p, HID_USAGE_PAGE_DESKTOP);
  hid_usage(&p, HID_USAGE_DESKTOP_MOUSE);
  hid_collection(&p, HID_COLLECTION_APPLICATION);
  
    hid_report_id(&p, reportID);

    hid_usage(&p, HID_USAGE_DESKTOP_POINTER);
    hid_collection(&p, HID_COLLECTION_PHYSICAL);
      hid_usage_page(&p, HID_USAGE_PAGE_BUTTON);
        hid_usage_min(&p, 1);
        hid_usage_max(&p, 5);
        hid_logical_min(&p, 0);
        hid_logical_max(&p, 1);
        hid_report_count(&p, 5);
        hid_report_size(&p, 1);
        hid_input(&p, HID_DATA | HID_VARIABLE | HID_ABSOLUTE);

        hid_report_count(&p, 1);
        hid_report_size(&p, 3);
        hid_input(&p, HID_CONSTANT);

      hid_usage_page(&p, HID_USAGE_PAGE_DESKTOP);
        hid_usage(&p, HID_USAGE_DESKTOP_X);
        hid_usage(&p, HID_USAGE_DESKTOP_Y);
        hid_usage_min(&p, 129);
        hid_usage_max(&p, 127);
        hid_report_count(&p, 2);
        hid_report_size(&p, 8);
        hid_input(&p, HID_DATA | HID_VARIABLE | HID_RELATIVE);

        hid_usage(&p, HID_USAGE_DESKTOP_WHEEL);
        hid_usage_min(&p, 129);
        hid_usage_max(&p, 127);
        hid_report_count(&p, 1);
        hid_report_size(&p, 8);
        hid_input(&p, HID_DATA | HID_VARIABLE | HID_RELATIVE);

      hid_usage_page(&p, HID_USAGE_PAGE_CONSUMER);
        hid_usage(&p, HID_USAGE_CONSUMER_AC_PAN);
        hid_usage_min(&p, 129);
        hid_usage_max(&p, 127);
        hid_report_count(&p, 1);
        hid_report_size(&p, 8);
        hid_input(&p, HID_DATA | HID_VARIABLE | HID_RELATIVE);

      hid_collection_end(&p);
    hid_collection_end(&p);

  total_bits[reportID] = bits;
  *bufferSize = p - buffer;

}
