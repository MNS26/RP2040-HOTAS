#define TUD_HID_REPORT_DESC_AXIS(...)                              \
HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP                       ),\
HID_USAGE       ( HID_USAGE_DESKTOP_MULTI_AXIS_CONTROLLER      ),\
HID_COLLECTION  ( HID_COLLECTION_APPLICATION                   ),\
/* Report ID if any */\
/*variadic argument*/\
      __VA_ARGS__ \
      HID_USAGE         ( HID_USAGE_DESKTOP_X                    ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_Y                    ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_Z                    ),\
      HID_LOGICAL_MIN   ( 0                                      ),\
      HID_LOGICAL_MAX_N ( 1023, 2                                ),\
      HID_REPORT_SIZE   ( 10                                     ),\
      HID_REPORT_COUNT  ( 3                                      ),\
      HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
\
      HID_REPORT_SIZE   ( 2                       /* padding */  ),\
      HID_REPORT_COUNT  ( 1                                      ),\
      HID_INPUT         ( HID_DATA | HID_ARRAY | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
\
      HID_USAGE         ( HID_USAGE_DESKTOP_RX                   ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_RY                   ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_RZ                   ),\
      HID_LOGICAL_MIN   ( 0                                      ),\
      HID_LOGICAL_MAX_N ( 1023, 2                                ),\
      HID_REPORT_SIZE   ( 10                                     ),\
      HID_REPORT_COUNT  ( 3                                      ),\
      HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
\
      HID_REPORT_SIZE   ( 2                       /* padding */  ),\
      HID_REPORT_COUNT  ( 1                                      ),\
      HID_INPUT         ( HID_DATA | HID_ARRAY | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
\
      HID_USAGE         ( HID_USAGE_DESKTOP_SLIDER               ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_DIAL                 ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_WHEEL                ),\
      HID_LOGICAL_MIN   ( 0                                      ),\
      HID_LOGICAL_MAX_N ( 1023, 2                                ),\
      HID_REPORT_SIZE   ( 10                                     ),\
      HID_REPORT_COUNT  ( 3                                      ),\
      HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
\
      HID_REPORT_SIZE   ( 2                       /* padding */  ),\
      HID_REPORT_COUNT  ( 1                                      ),\
      HID_INPUT         ( HID_DATA | HID_ARRAY | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
\
      HID_USAGE_PAGE    ( HID_USAGE_PAGE_DESKTOP                 ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_HAT_SWITCH           ),\
      HID_LOGICAL_MIN   ( 1                                      ),\
      HID_LOGICAL_MAX   ( 8                                      ),\
      HID_PHYSICAL_MIN  ( 0                                      ),\
      HID_PHYSICAL_MAX_N( 315, 2                                 ),\
      HID_REPORT_SIZE   ( 8                                      ),\
      HID_REPORT_COUNT  ( 1                                      ),\
      HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NULL_STATE ),\
\
      HID_USAGE_PAGE    ( HID_USAGE_PAGE_BUTTON                  ),\
      HID_USAGE_MIN     ( 1                                      ),\
      HID_USAGE_MAX     ( 16                                      ),\
      HID_LOGICAL_MIN   ( 0                                      ),\
      HID_LOGICAL_MAX   ( 1                                      ),\
      HID_REPORT_SIZE   ( 1                                      ),\
      HID_REPORT_COUNT  ( 16                                      ),\
      HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
\
      HID_USAGE      ( HID_USAGE_DESKTOP_POINTER )                ,\
      HID_COLLECTION ( HID_COLLECTION_PHYSICAL   )                ,\
            HID_USAGE_PAGE    ( HID_USAGE_PAGE_GAME              ),\
            HID_USAGE         ( HID_USAGE_GAME_MOVE_UD           ),\
            HID_USAGE         ( HID_USAGE_GAME_MOVE_LR           ),\
            HID_LOGICAL_MIN   ( 0                                ),\
            HID_LOGICAL_MAX   ( 15                               ),\
            HID_REPORT_SIZE   ( 2                                ),\
            HID_REPORT_COUNT  ( 1                                ),\
            HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
      HID_COLLECTION_END                                          ,\
HID_COLLECTION_END

#define TUD_HID_REPORT_DESC_BUTTONS(...)                  \
  HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP                       ),\
  HID_USAGE       ( HID_USAGE_DESKTOP_JOYSTICK                   ),\
  HID_COLLECTION  ( HID_COLLECTION_APPLICATION                   ),\
      /* Report ID if any */\
      /*variadic argument*/\
    __VA_ARGS__ \
    HID_USAGE        ( HID_USAGE_DESKTOP_POINTER                 ),\
    HID_COLLECTION   ( HID_COLLECTION_PHYSICAL                   ),\
      HID_USAGE_PAGE    ( HID_USAGE_PAGE_BUTTON                  ),\
      HID_USAGE_MIN     ( 1                                      ),\
      HID_USAGE_MAX     ( 80                                     ),\
      HID_LOGICAL_MIN   ( 0                                      ),\
      HID_LOGICAL_MAX   ( 1                                      ),\
      HID_REPORT_SIZE   ( 1                                      ),\
      HID_REPORT_COUNT  ( 80                                     ),\
      HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
  HID_COLLECTION_END,\
HID_COLLECTION_END


//      HID_REPORT_SIZE   ( 2                       /* padding */  ),\
      HID_REPORT_COUNT  ( 1                                      ),\
      HID_INPUT         ( HID_DATA | HID_ARRAY | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),*/\
\
      HID_USAGE         ( HID_USAGE_DESKTOP_VX                   ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_VY                   ),\
      HID_USAGE         ( HID_USAGE_DESKTOP_VZ                   ),\
      HID_LOGICAL_MIN   ( 0                                      ),\
      HID_LOGICAL_MAX_N ( 1023, 2                                ),\
      HID_REPORT_SIZE   ( 10                                     ),\
      HID_REPORT_COUNT  ( 3                                      ),\
      HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
      HID_REPORT_SIZE   ( 2                       /* padding */  ),\
      HID_REPORT_COUNT  ( 1                                      ),\
      HID_INPUT         ( HID_DATA | HID_ARRAY | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\

/*
bit layout
   axis      axis      axis      axis       axis       axis        slider    dial     wheel                                buttons                                               HAT   game LR/UD
|      |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
XXXXXXXXXXYYYYYYYYYYZZZZZZZZZZRzRzRzRzRzRxRxRxRxRxRyRyRyRyRySSSSSSSSSSDDDDDDDDDDWWWWWWWWWWHHHH..LRLRUDUDFBFBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
XXXXXXXXXXYYYYYYYYYYZZZZZZZZZZ..RzRzRzRzRzRxRxRxRxRxRyRyRyRyRy..SSSSSSSSSSDDDDDDDDDDWWWWWWWWWW..BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBHHHH....LRLRUDUDFBFB
|  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
1      8      16      24      32      40      48      56      64      72      80      88      96      104     112     120     128     136     144     152     160     168     176     184     193
*/

// HID Generic Input & Output
// - 1st parameter is report size (mandatory)
// - 2nd parameter is report id HID_REPORT_ID(n) (optional)
#define TUD_HID_REPORT_DESC_GENERIC_INOUT(report_size, ...) \
  HID_USAGE_PAGE_N ( HID_USAGE_PAGE_VENDOR, 2   ),\
    HID_USAGE        ( 0x01                       ),\
    HID_COLLECTION   ( HID_COLLECTION_APPLICATION ),\
      /* Report ID if any */\
      __VA_ARGS__ \
      /* Input */ \
      HID_USAGE       ( 0x02                                   ),\
      HID_LOGICAL_MIN ( 0x00                                   ),\
      HID_LOGICAL_MAX_N ( 0xff, 2                              ),\
      HID_REPORT_SIZE ( 8                                      ),\
      HID_REPORT_COUNT( report_size                            ),\
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),\
      /* Output */ \
      HID_USAGE       ( 0x03                                    ),\
      HID_LOGICAL_MIN ( 0x00                                    ),\
      HID_LOGICAL_MAX_N ( 0xff, 2                               ),\
      HID_REPORT_SIZE ( 8                                       ),\
      HID_REPORT_COUNT( report_size                             ),\
      HID_OUTPUT      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ),\
HID_COLLECTION_END
