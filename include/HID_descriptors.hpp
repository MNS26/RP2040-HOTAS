uint AddHeader(uint8_t* buff, size_t offset, uint8_t repId) {

  uint8_t usagepage[] = {
  	HID_USAGE_PAGE        ( HID_USAGE_PAGE_DESKTOP        ),
    HID_USAGE             ( HID_USAGE_DESKTOP_JOYSTICK    ),
    HID_COLLECTION        ( HID_COLLECTION_APPLICATION    ),
  };
  memcpy(buff+offset, usagepage, sizeof(usagepage));
  offset += sizeof(usagepage);

  if (repId > 0) {
    uint8_t reportid[] = {
      HID_REPORT_ID (repId)
    };
    memcpy(buff+offset, reportid, sizeof(reportid));
    offset += sizeof(reportid);
  }

  uint8_t usagepage2[] = {
    HID_USAGE             ( HID_USAGE_DESKTOP_POINTER     ),
    HID_COLLECTION        ( HID_COLLECTION_PHYSICAL       ),
  };
  memcpy(buff+offset, usagepage2, sizeof(usagepage2));
  offset += sizeof(usagepage2);

  return offset;
}

uint8_t AddCollectionEnd(uint8_t* buff, size_t offset) {
  uint8_t CollectionEnd[] = {
    HID_COLLECTION_END,
    HID_COLLECTION_END
  };
  memcpy(buff+offset, CollectionEnd, sizeof(CollectionEnd));
  offset += sizeof(CollectionEnd);
  return offset;
}

uint AddAxis(uint8_t* buff, size_t offset, uint8_t usage, uint8_t count, uint8_t bitResolution) {

  if (count > 8) {
    count = 8;
  }
  // Calculate how many bytes this value will be
  uint8_t ValueSize = (int)(std::ceil((float)(bitResolution)/8));

  // Calculate the amount of padding needed to get it byte aligned (fuck you windows for needing this)
  // We take the remainder of the mod and subtrack it from 8 (aka 1 byte)
  // but since 8 - 0 is still 8 we mod it again so we dont have a extra byte dangling behind it
  uint8_t _padding = (8 - (11 % 8)) % 8; 

  // Get the maximum input value by shifting a 1 X times and subtacting a 1 to set them all
  uint32_t logical_max = (1 << 11) - 1;

  uint8_t usagepage[] = { 
    HID_USAGE_PAGE      ( HID_USAGE_PAGE_DESKTOP        ),
  };
  memcpy(buff+offset, usagepage, sizeof(usagepage));
  offset += sizeof(usagepage);

  for (int i = 0; i < count; i++) {

    uint8_t usagepage[] = { 
      HID_USAGE ( (uint8_t)(HID_USAGE_DESKTOP_X+i) ),
      HID_LOGICAL_MIN( 0 )
    };
    memcpy(buff+offset, usagepage, sizeof(usagepage));
    offset += sizeof(usagepage);

    switch (ValueSize)
    {
      case 1: {
        uint8_t usagepage[] = { 
          HID_LOGICAL_MAX_N ( (uint8_t)logical_max, 1 ),
        };
        memcpy(buff+offset, usagepage, sizeof(usagepage));
        offset += sizeof(usagepage);
        break;
      }
      case 2: {
        uint8_t usagepage[] = { 
          HID_LOGICAL_MAX_N ( (uint16_t)logical_max, 2 ),
        };
        memcpy(buff+offset, usagepage, sizeof(usagepage));
        offset += sizeof(usagepage);
        break;
      }
      case 3: 
      case 4: {
        uint8_t usagepage[] = { 
          HID_LOGICAL_MAX_N ( (uint32_t)logical_max, 3 ),
        };
        memcpy(buff+offset, usagepage, sizeof(usagepage));
        offset += sizeof(usagepage);
        break;
      }
      default: {
        uint8_t usagepage[] = {};
        memcpy(buff+offset, usagepage, sizeof(usagepage));
        offset += sizeof(usagepage);
        break;
      }
    }

    if (ValueSize > 0)
    {
      uint8_t usagepage[] ={
        HID_PHYSICAL_MIN_N( -32768, 2                     ),
        HID_PHYSICAL_MAX_N( 32767, 2                      ),
        HID_REPORT_COUNT  ( 1                             ),
        HID_REPORT_SIZE   ( bitResolution                 ),
        HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
      };
      memcpy(buff+offset, usagepage, sizeof(usagepage));
      offset += sizeof(usagepage);
    }

    if (_padding > 0) {
      uint8_t padding[] ={
        HID_REPORT_COUNT  ( 1            ),
        HID_REPORT_SIZE   ( _padding     ),
        HID_INPUT         ( HID_CONSTANT ),
      };
      memcpy(buff+offset, padding, sizeof(padding));
      offset += sizeof(padding);
    }
  }

  return offset;
};

uint AddButtons(uint8_t* buff, size_t offset, uint8_t count) {

  if (count > 32) {
    count = 32;
  }
  // Calculate the amount of padding needed to get it byte aligned (fuck you windows for needing this)
  // We take the remainder of the mod and subtrack it from 8 (aka 1 byte)
  // but since 8 - 0 is still 8 we mod it again so we dont have a extra byte dangling behind it
  uint8_t _padding = (8 - (count % 8)) % 8; 

  uint8_t usagepage[] = {
    HID_USAGE_PAGE      ( HID_USAGE_PAGE_BUTTON         ),
      HID_USAGE_MIN     ( 1                             ),
      HID_USAGE_MAX     ( count                         ),
      HID_LOGICAL_MIN   ( 0                             ),
      HID_LOGICAL_MAX   ( 1                             ),
      HID_REPORT_COUNT  ( count                         ),
      HID_REPORT_SIZE   ( 1                             ),
      HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_LINEAR|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
  };
  memcpy(buff+offset, usagepage, sizeof(usagepage));
  offset += sizeof(usagepage);

  if (_padding > 0) {
    uint8_t padding[] ={
      HID_REPORT_COUNT  ( 1            ),
      HID_REPORT_SIZE   ( _padding     ),
      HID_INPUT         ( HID_CONSTANT ),
    };
    memcpy(buff+offset, padding, sizeof(padding));
    offset += sizeof(padding);
  }

  return offset;
};

uint AddHats(uint8_t* buff, size_t offset, uint8_t count) {

  // Windows only sees maximum 4 hats on one device
  if (count > 4) {
    count = 4;
  }

  // 
  uint8_t _padding = bitRead(count,0); 

  uint8_t usagepage[] = {
    HID_USAGE_PAGE      ( HID_USAGE_PAGE_DESKTOP       ),
  };
  memcpy(buff+offset, usagepage, sizeof(usagepage));
  offset += sizeof(usagepage);
  
  for (int i = 0; i < count; i++) {
    uint8_t hat[] = {
      HID_USAGE ( HID_USAGE_DESKTOP_HAT_SWITCH)
    };
    memcpy(buff+offset, hat, sizeof(hat));
    offset += sizeof(hat);
  }

  uint8_t usagepage2[] = {
      HID_LOGICAL_MIN   ( 1                            ),
      HID_LOGICAL_MAX   ( 8                            ),
      HID_PHYSICAL_MIN  ( 0                            ),
      HID_PHYSICAL_MAX_N( 315, 2                       ),
      HID_REPORT_COUNT  ( count                        ),
      HID_REPORT_SIZE   ( 4                            ),
      HID_INPUT         ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE|HID_WRAP_NO|HID_PREFERRED_STATE|HID_NO_NULL_POSITION ),
  };
  memcpy(buff+offset, usagepage2, sizeof(usagepage2));
  offset += sizeof(usagepage2);

  if (_padding) {
    uint8_t padding[] = {
      HID_REPORT_COUNT  ( 1            ),
      HID_REPORT_SIZE   ( 4            ),
      HID_INPUT         ( HID_CONSTANT ),
    };
    memcpy(buff+offset, padding, sizeof(padding));
    offset += sizeof(padding);
  }

  return offset;
};


/*
bit layout
  . = padding bits
  B = buttons (32 x 1 bit)
  X = X (axis 11 bit)
  Y = Y (axis 11 bit)
  Z = Z (axis 11 bit)
  x = Rx (axis 11 bit)
  y = Ry (axis 11 bit)
  z = Rz (axis 11 bit)
  S = slider (axis 11 bit)
  D = dial (axis 11 bit)
  H1/2 = HAT (Dpad 4 bit)
|      |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBXXXXXXXXXXX.....YYYYYYYYYYY.....ZZZZZZZZZZZ.....xxxxxxxxxxx.....yyyyyyyyyyy.....zzzzzzzzzzz.....SSSSSSSSSSS.....DDDDDDDDDDD.....H1H1H2H2
|  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
1      8      16      24      32      40      48      56      64      72      80      88      96      104     112     120     128     136     144     152     160     168     176     184     192     200     208     216     224     232     240     248     256
*/


// Keyboard Report Descriptor Template

uint16_t HID_KEYBOARD_rep[] = {

  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                    ,
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD )                    ,
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                    ,
    /* Report ID if any */
  HID_REPORT_ID         ( 1                             ),
    /* 8 bits Modifier Keys (Shift, Control, Alt) */
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,
      HID_USAGE_MIN    ( 224                                    )  ,
      HID_USAGE_MAX    ( 231                                    )  ,
      HID_LOGICAL_MIN  ( 0                                      )  ,
      HID_LOGICAL_MAX  ( 1                                      )  ,
      HID_REPORT_COUNT ( 8                                      )  ,
      HID_REPORT_SIZE  ( 1                                      )  ,
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,
      /* 8 bit reserved */ \
      HID_REPORT_COUNT ( 1                                      )  ,
      HID_REPORT_SIZE  ( 8                                      )  ,
      HID_INPUT        ( HID_CONSTANT                           )  ,
    /* Output 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED                   )       ,
      HID_USAGE_MIN    ( 1                                       ) ,
      HID_USAGE_MAX    ( 5                                       ) ,
      HID_REPORT_COUNT ( 5                                       ) ,
      HID_REPORT_SIZE  ( 1                                       ) ,
      HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,
      /* led padding */ 
      HID_REPORT_COUNT ( 1                                       ) ,
      HID_REPORT_SIZE  ( 3                                       ) ,
      HID_OUTPUT       ( HID_CONSTANT                            ) ,
    /* 6-byte Keycodes */ 
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,
      HID_USAGE_MIN    ( 0                                   )     ,
      HID_USAGE_MAX_N  ( 255, 2                              )     ,
      HID_LOGICAL_MIN  ( 0                                   )     ,
      HID_LOGICAL_MAX_N( 255, 2                              )     ,
      HID_REPORT_COUNT ( 6                                   )     ,
      HID_REPORT_SIZE  ( 8                                   )     ,
      HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )     ,
  HID_COLLECTION_END
};

// Mouse Report Descriptor Template
uint8_t HID_MOUSE_rep[] = {
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP      )                   ,
  HID_USAGE      ( HID_USAGE_DESKTOP_MOUSE     )                   ,
  HID_COLLECTION ( HID_COLLECTION_APPLICATION  )                   ,
    HID_REPORT_ID         ( 1                             ),
    /* Report ID if any */
    HID_USAGE      ( HID_USAGE_DESKTOP_POINTER )                   ,
    HID_COLLECTION ( HID_COLLECTION_PHYSICAL   )                   ,
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_BUTTON  )                   ,
        HID_USAGE_MIN   ( 1                                      ) ,
        HID_USAGE_MAX   ( 5                                      ) ,
        HID_LOGICAL_MIN ( 0                                      ) ,
        HID_LOGICAL_MAX ( 1                                      ) ,
        /* Left, Right, Middle, Backward, Forward buttons */ \
        HID_REPORT_COUNT( 5                                      ) ,
        HID_REPORT_SIZE ( 1                                      ) ,
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,
        /* 3 bit padding */ 
        HID_REPORT_COUNT( 1                                      ) ,
        HID_REPORT_SIZE ( 3                                      ) ,
        HID_INPUT       ( HID_CONSTANT                           ) ,
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP )                   ,
        /* X, Y position [-127, 127] */ 
        HID_USAGE       ( HID_USAGE_DESKTOP_X                    ) ,
        HID_USAGE       ( HID_USAGE_DESKTOP_Y                    ) ,
        HID_LOGICAL_MIN ( 0x81                                   ) ,
        HID_LOGICAL_MAX ( 0x7f                                   ) ,
        HID_REPORT_COUNT( 2                                      ) ,
        HID_REPORT_SIZE ( 8                                      ) ,
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ) ,
        /* Verital wheel scroll [-127, 127] */ 
        HID_USAGE       ( HID_USAGE_DESKTOP_WHEEL                )  ,
        HID_LOGICAL_MIN ( 0x81                                   )  ,
        HID_LOGICAL_MAX ( 0x7f                                   )  ,
        HID_REPORT_COUNT( 1                                      )  ,
        HID_REPORT_SIZE ( 8                                      )  ,
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE )  ,
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_CONSUMER ),
       /* Horizontal wheel scroll [-127, 127] */ 
        HID_USAGE_N     ( HID_USAGE_CONSUMER_AC_PAN, 2           ),
        HID_LOGICAL_MIN ( 0x81                                   ),
        HID_LOGICAL_MAX ( 0x7f                                   ),
        HID_REPORT_COUNT( 1                                      ),
        HID_REPORT_SIZE ( 8                                      ),
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ),
    HID_COLLECTION_END                                            ,
  HID_COLLECTION_END
};


// Keyboard Report Descriptor Template
#define rTUD_HID_REPORT_DESC_KEYBOARD(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                    ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD )                    ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                    ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* 8 bits Modifier Keys (Shift, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 224                                    )  ,\
      HID_USAGE_MAX    ( 231                                    )  ,\
      HID_LOGICAL_MIN  ( 0                                      )  ,\
      HID_LOGICAL_MAX  ( 1                                      )  ,\
      HID_REPORT_COUNT ( 8                                      )  ,\
      HID_REPORT_SIZE  ( 1                                      )  ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
      /* 8 bit reserved */ \
      HID_REPORT_COUNT ( 1                                      )  ,\
      HID_REPORT_SIZE  ( 8                                      )  ,\
      HID_INPUT        ( HID_CONSTANT                           )  ,\
    /* Output 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED                   )       ,\
      HID_USAGE_MIN    ( 1                                       ) ,\
      HID_USAGE_MAX    ( 5                                       ) ,\
      HID_REPORT_COUNT ( 5                                       ) ,\
      HID_REPORT_SIZE  ( 1                                       ) ,\
      HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
      /* led padding */ \
      HID_REPORT_COUNT ( 1                                       ) ,\
      HID_REPORT_SIZE  ( 3                                       ) ,\
      HID_OUTPUT       ( HID_CONSTANT                            ) ,\
    /* 6-byte Keycodes */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 0                                   )     ,\
      HID_USAGE_MAX_N  ( 255, 2                              )     ,\
      HID_LOGICAL_MIN  ( 0                                   )     ,\
      HID_LOGICAL_MAX_N( 255, 2                              )     ,\
      HID_REPORT_COUNT ( 6                                   )     ,\
      HID_REPORT_SIZE  ( 8                                   )     ,\
      HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )     ,\
  HID_COLLECTION_END



#define TUD_HID_REPORT_DESC_MOUSE(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP      )                   ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_MOUSE     )                   ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION  )                   ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    HID_USAGE      ( HID_USAGE_DESKTOP_POINTER )                   ,\
    HID_COLLECTION ( HID_COLLECTION_PHYSICAL   )                   ,\
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_BUTTON  )                   ,\
        HID_USAGE_MIN   ( 1                                      ) ,\
        HID_USAGE_MAX   ( 5                                      ) ,\
        HID_LOGICAL_MIN ( 0                                      ) ,\
        HID_LOGICAL_MAX ( 1                                      ) ,\
        /* Left, Right, Middle, Backward, Forward buttons */ \
        HID_REPORT_COUNT( 5                                      ) ,\
        HID_REPORT_SIZE ( 1                                      ) ,\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
        /* 3 bit padding */ \
        HID_REPORT_COUNT( 1                                      ) ,\
        HID_REPORT_SIZE ( 3                                      ) ,\
        HID_INPUT       ( HID_CONSTANT                           ) ,\
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP )                   ,\
        /* X, Y position [-127, 127] */ \
        HID_USAGE       ( HID_USAGE_DESKTOP_X                    ) ,\
        HID_USAGE       ( HID_USAGE_DESKTOP_Y                    ) ,\
        HID_LOGICAL_MIN ( 0x81                                   ) ,\
        HID_LOGICAL_MAX ( 0x7f                                   ) ,\
        HID_REPORT_COUNT( 2                                      ) ,\
        HID_REPORT_SIZE ( 8                                      ) ,\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ) ,\
        /* Verital wheel scroll [-127, 127] */ \
        HID_USAGE       ( HID_USAGE_DESKTOP_WHEEL                )  ,\
        HID_LOGICAL_MIN ( 0x81                                   )  ,\
        HID_LOGICAL_MAX ( 0x7f                                   )  ,\
        HID_REPORT_COUNT( 1                                      )  ,\
        HID_REPORT_SIZE ( 8                                      )  ,\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE )  ,\
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_CONSUMER ), \
       /* Horizontal wheel scroll [-127, 127] */ \
        HID_USAGE_N     ( HID_USAGE_CONSUMER_AC_PAN, 2           ), \
        HID_LOGICAL_MIN ( 0x81                                   ), \
        HID_LOGICAL_MAX ( 0x7f                                   ), \
        HID_REPORT_COUNT( 1                                      ), \
        HID_REPORT_SIZE ( 8                                      ), \
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), \
    HID_COLLECTION_END                                            , \
  HID_COLLECTION_END \







// HID Generic Input & Output
// - 1st parameter is report size (mandatory)
// - 2nd parameter is report id HID_REPORT_ID(n) (optional)
#define TUD_HID_REPORT_DESC_GENERIC_INOUT(report_size, ...)\
  HID_USAGE_PAGE   ( HID_USAGE_PAGE_VENDOR       ),\
  HID_USAGE        ( HID_USAGE_DESKTOP_UNDEFINED ),\
  HID_REPORT_SIZE  ( 8                           ),\
  HID_COLLECTION   ( HID_COLLECTION_LOGICAL      ),\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* Input */ \
    HID_REPORT_COUNT( report_size                        ),\
    HID_USAGE       ( 0x01                               ),\
    HID_FEATURE     ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE ),\
    /* Output */ \
    HID_REPORT_COUNT( report_size                        ),\
    HID_USAGE       ( 0x01                               ),\
    HID_FEATURE     ( HID_DATA|HID_VARIABLE|HID_ABSOLUTE ),\
  HID_COLLECTION_END

/* System Control Report Descriptor Template
 * 0x00 - do nothing
 * 0x01 - Power Off
 * 0x02 - Standby
 * 0x03 - Wake Host
 */
#define TUD_HID_REPORT_DESC_SYSTEM_CONTROL(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )        ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_SYSTEM_CONTROL )        ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION       )        ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* 2 bit system power control */ \
    HID_LOGICAL_MIN  ( 1                                   ) ,\
    HID_LOGICAL_MAX  ( 3                                   ) ,\
    HID_REPORT_COUNT ( 1                                   ) ,\
    HID_REPORT_SIZE  ( 2                                   ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_SYSTEM_SLEEP      ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_SYSTEM_WAKE_UP    ) ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ) ,\
    /* 6 bit padding */ \
    HID_REPORT_COUNT ( 1                                   ) ,\
    HID_REPORT_SIZE  ( 6                                   ) ,\
    HID_INPUT        ( HID_CONSTANT                        ) ,\
  HID_COLLECTION_END \

