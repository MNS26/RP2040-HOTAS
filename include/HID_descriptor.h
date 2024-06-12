#include <Arduino.h>


uint8_t generateDeviceDescriptor(uint8_t* buff, uint8_t offset, uint8_t reportID, uint8_t AxisCount, uint8_t AxisResolution, uint8_t ButtonCount, uint8_t HatCount) {
  extern uint8_t hid_usage_page_val;
  extern uint8_t hid_usage_val;
  // Calculate how many bytes this value will be
  uint8_t AxisByteSize = (int)(std::ceil((float)(AxisResolution)/8));

  // Set it to 4 in case we are larger than 2 since thats the next available typesize
  if (AxisByteSize > 3) {AxisByteSize = 3;}

  // Calculate the amount of padding needed to get it byte aligned (fuck you windows for needing this)
  // We take the remainder of the mod and subtrack it from 8 (aka 1 byte)
  // but since 8 - 0 is still 8 we mod it again so we dont have a extra byte dangling behind it
  uint8_t paddingAxis = (8 - (AxisResolution % 8)) % 8; 
  uint8_t paddingButtons = (8 - (ButtonCount % 8)) % 8; 
  uint8_t paddingHats = (8 - (HatCount * 4 % 8)) % 8; 

  // Get the maximum input value by shifting a 1 X times and subtacting a 1 to set them all
  uint32_t logical_max = (1 << AxisResolution) - 1;

uint8_t header[] = {
//  	HID_USAGE_PAGE        ( HID_USAGE_PAGE_DESKTOP        ),
//    HID_USAGE             ( HID_USAGE_DESKTOP_JOYSTICK    ),
    HID_USAGE_PAGE        ( hid_usage_page_val ),
    HID_USAGE             ( hid_usage_val ),
    HID_COLLECTION        ( HID_COLLECTION_APPLICATION    ),
     HID_REPORT_ID         (reportID),
  	 HID_USAGE_PAGE        ( HID_USAGE_PAGE_DESKTOP        ),
     HID_USAGE             ( HID_USAGE_DESKTOP_POINTER     ),
     HID_COLLECTION        ( HID_COLLECTION_PHYSICAL       ),
  };
  memcpy(buff+offset, header, sizeof(header));
  offset += sizeof(header);

  if (AxisCount > 0 ) {
    uint8_t usagepage[] = {
      HID_USAGE_PAGE     ( HID_USAGE_PAGE_DESKTOP),
    };
    memcpy(buff+offset, usagepage, sizeof(usagepage));
    offset += sizeof(usagepage);

    for (int i = 0; i < AxisCount; i++) {
      uint8_t usagepage[] = { 
        HID_USAGE ( (uint8_t)(HID_USAGE_DESKTOP_X+i) ),
      };
      memcpy(buff+offset, usagepage, sizeof(usagepage));
      offset += sizeof(usagepage);
      if (AxisByteSize < 2 ) {
        uint8_t usagepage[] = { 
          HID_LOGICAL_MIN( 0 ),
          HID_LOGICAL_MAX_N ( (uint8_t)logical_max, 1 ),
          HID_PHYSICAL_MIN_N( -32768, 2                     ),
          HID_PHYSICAL_MAX_N( 32767, 2                      ),
          HID_REPORT_COUNT  ( 1                             ),
          HID_REPORT_SIZE   ( AxisResolution                 ),
          HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
        };
        memcpy(buff+offset, usagepage, sizeof(usagepage));
        offset += sizeof(usagepage);
      }
      if (AxisByteSize == 2) {
        uint8_t usagepage[] = { 
          HID_LOGICAL_MIN( 0 ),
          HID_LOGICAL_MAX_N ( (uint16_t)logical_max, 2 ),
          HID_PHYSICAL_MIN_N( -32768, 2                     ),
          HID_PHYSICAL_MAX_N( 32767, 2                      ),
          HID_REPORT_COUNT  ( 1                             ),
          HID_REPORT_SIZE   ( AxisResolution                 ),
          HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
        };
        memcpy(buff+offset, usagepage, sizeof(usagepage));
        offset += sizeof(usagepage);
      }
      if (AxisByteSize > 2) {
        uint8_t usagepage[] = { 
          HID_LOGICAL_MIN( 0 ),
          HID_LOGICAL_MAX_N ( (uint32_t)logical_max, 3 ),
          HID_PHYSICAL_MIN_N( -32768, 2                     ),
          HID_PHYSICAL_MAX_N( 32767, 2                      ),
          HID_REPORT_COUNT  ( 1                             ),
          HID_REPORT_SIZE   ( AxisResolution                 ),
          HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
        };
        memcpy(buff+offset, usagepage, sizeof(usagepage));
        offset += sizeof(usagepage);
      }

      if (paddingAxis > 0) {
        uint8_t padding[] ={
          HID_REPORT_COUNT  ( 1            ),
          HID_REPORT_SIZE   ( paddingAxis     ),
          HID_INPUT         ( HID_CONSTANT ),
        };
        memcpy(buff+offset, padding, sizeof(padding));
        offset += sizeof(padding);
      }
    }
  }

  
  if (HatCount > 0) {
    uint8_t usagepage2[] = {
      HID_USAGE_PAGE      ( HID_USAGE_PAGE_DESKTOP       ),
    };
    memcpy(buff+offset, usagepage2, sizeof(usagepage2));
    offset += sizeof(usagepage2);
    
    for (int i = 0; i < HatCount; i++) {
      uint8_t hat[] = {
        HID_USAGE ( HID_USAGE_DESKTOP_HAT_SWITCH)
      };
      memcpy(buff+offset, hat, sizeof(hat));
      offset += sizeof(hat);
    }
    uint8_t usagepage3[] = {
        HID_LOGICAL_MIN   ( 1                            ),
        HID_LOGICAL_MAX   ( 8                            ),
        HID_PHYSICAL_MIN  ( 0                            ),
        HID_PHYSICAL_MAX_N( 315, 2                       ),
        HID_REPORT_COUNT  ( HatCount                     ),
        HID_REPORT_SIZE   ( 4                            ),
        HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
    };
    memcpy(buff+offset, usagepage3, sizeof(usagepage3));
    offset += sizeof(usagepage3);

    if (paddingHats > 0) {
      uint8_t padding[] = {
        HID_REPORT_COUNT  ( 1            ),
        HID_REPORT_SIZE   ( paddingHats  ),
        HID_INPUT         ( HID_CONSTANT ),
      };
      memcpy(buff+offset, padding, sizeof(padding));
      offset += sizeof(padding);
    }
  }

  if (ButtonCount > 0) {
    uint8_t usagepage4[] = {
      HID_USAGE_PAGE      ( HID_USAGE_PAGE_BUTTON         ),
        HID_USAGE_MIN     ( 1                             ),
        HID_USAGE_MAX     ( ButtonCount                   ),
        HID_LOGICAL_MIN   ( 0                             ),
        HID_LOGICAL_MAX   ( 1                             ),
        HID_REPORT_COUNT  ( ButtonCount                   ),
        HID_REPORT_SIZE   ( 1                             ),
        HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
    };
    memcpy(buff+offset, usagepage4, sizeof(usagepage4));
    offset += sizeof(usagepage4);

    if (paddingButtons > 0) {
      uint8_t padding[] = {
        HID_REPORT_COUNT  ( 1            ),
        HID_REPORT_SIZE   ( paddingButtons),
        HID_INPUT         ( HID_CONSTANT ),
      };
      memcpy(buff+offset, padding, sizeof(padding));
      offset += sizeof(padding);
    }
  }

  uint8_t CollectionEnd[] = {
    HID_COLLECTION_END,
    HID_COLLECTION_END
  };
  memcpy(buff+offset, CollectionEnd, sizeof(CollectionEnd));
  offset += sizeof(CollectionEnd);
  return offset;
}

#define TUD_HID_REPORT_DESC_GAMEPAD_(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                 ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_JOYSTICK  )                 ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                 ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* 8 bit X, Y, Z, Rz, Rx, Ry (min -127, max 127 ) */ \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_DESKTOP                 ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+0                  ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+1                  ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+2                  ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+3                  ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+4                  ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+5                  ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+6                  ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X+7                  ) ,\
    HID_LOGICAL_MIN    ( 0                                      ) ,\
    HID_LOGICAL_MAX_N    ( 1<<11-1,2                                  ) ,\
    HID_REPORT_COUNT   ( 8                                      ) ,\
    HID_REPORT_SIZE    ( 11                                      ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* 8 bit DPad/Hat Button Map  */ \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_DESKTOP                 ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_HAT_SWITCH           ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_HAT_SWITCH           ) ,\
    HID_LOGICAL_MIN    ( 1                                      ) ,\
    HID_LOGICAL_MAX    ( 8                                      ) ,\
    HID_PHYSICAL_MIN   ( 0                                      ) ,\
    HID_PHYSICAL_MAX_N ( 315, 2                                 ) ,\
    HID_REPORT_COUNT   ( 2                                      ) ,\
    HID_REPORT_SIZE    ( 4                                      ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* 32 bit Button Map */ \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_BUTTON                  ) ,\
    HID_USAGE_MIN      ( 1                                      ) ,\
    HID_USAGE_MAX      ( 64                                     ) ,\
    HID_LOGICAL_MIN    ( 0                                      ) ,\
    HID_LOGICAL_MAX    ( 1                                      ) ,\
    HID_REPORT_COUNT   ( 64                                     ) ,\
    HID_REPORT_SIZE    ( 1                                      ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
  HID_COLLECTION_END