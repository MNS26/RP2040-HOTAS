/// HID Gamepad Protocol Report.


typedef struct TU_ATTR_PACKED
{
  uint64_t buttons     ;  ///< Buttons mask for currently pressed buttons
  uint8_t  hat1     :4 ;   ///< Buttons mask for currently pressed buttons in the DPad/hat
  uint8_t  hat2     :4 ;   ///< Buttons mask for currently pressed buttons in the DPad/hat
  int16_t  x           ;  ///< Delta x  movement of left analog-stick
  int16_t  y           ;  ///< Delta y  movement of left analog-stick
  int16_t  z           ;  ///< Delta z  movement of analog right trigger
  int16_t  rx          ;  ///< Delta Rz movement of right analog-joystick
  int16_t  ry          ;  ///< Delta Rx movement of analog left trigger
  int16_t  rz          ;  ///< Delta Rz movement of right analog-joystick
  int16_t  slider      ;  ///< Delta Ry movement of analog right trigger
  int16_t  dial        ;  ///< Delta Ry movement of analog right trigger

 
}hid_Joystick_report_t;


