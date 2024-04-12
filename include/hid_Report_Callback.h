//DATA TYPE
enum DataType
{
  TypeAxis = 1,
  TypeButtons,
  TypeHat,
  TypePadding,
}; 

/*
  =============
    Callbacks  
  =============
*/
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  switch (report_type)
  {
    case (hid_report_type_t)HID_REPORT_TYPE_INVALID:
    case (hid_report_type_t)HID_REPORT_TYPE_OUTPUT:
    break;

    case (hid_report_type_t)HID_REPORT_TYPE_INPUT:
    case (hid_report_type_t)HID_REPORT_TYPE_FEATURE:	
      {
        Serial.print("get_report type: ");
        Serial.println((int)report_type);
        Serial.print("get_report id: ");
        Serial.println((int)report_id);
    
        //for (int i = 0; i<reqlen; i++) {	
        //	buffer[i] = 0x00;
        //}
        if (report_id == 0){
          for (int i = 0; i <DeviceCountJoystick; i++)
          {
          buffer[0] = i+1;
          buffer[i+1] = i+1;
//					buffer[2] = RID_JOYSTICK2;
          }
        }
        if (report_id == 1) {
        

          //buffer[0] = 9;

          buffer[1] = TypeButtons;
          buffer[2] = 1;
          buffer[3] = 32;

          for (int i = 0; i < AxisCount; i++) {
            buffer[(i*6)+4]  = TypeAxis;
            buffer[(i*6)+5]  = 11;
            buffer[(i*6)+7]  = 1;
            buffer[(i*6)+8]  = TypePadding;
            buffer[(i*6)+9]  = 5;
            buffer[(i*6)+10] = 1;					
          }
          //buffer[4] = Axis;
          //buffer[5] = 11;
          //buffer[6] = 1;

          //buffer[7] = Padding;
          //buffer[8] = 5;
          //buffer[9] = 1;
          buffer[0] = sizeof(buffer)-1;
        }
        if (report_id == 2){
          //buffer[0] = 9;


          for (int i = 0; i < AxisCount; i++) {
            buffer[(i*6)]  = TypeAxis;
            buffer[(i*6)+1]  = 11;
            buffer[(i*6)+2]  = 1;
            buffer[(i*6)+3]  = TypePadding;
            buffer[(i*6)+4]  = 5;
            buffer[(i*6)+5] = 1;					
          }
          buffer[1] = TypeButtons;
          buffer[2] = 1;
          buffer[3] = 32;
          //buffer[4] = Axis;
          //buffer[5] = 11;
          //buffer[6] = 1;

          //buffer[7] = Padding;
          //buffer[8] = 5;
          //buffer[9] = 1;
          buffer[0] = sizeof(buffer)-1;
        }
        //if (report_id == RID_KEYBOARD)
        //{
        //	for (int i = 0; i<reqlen; i++) {	
        //		buffer[i] = 0xf0;
        //	}
        //}
        //if (report_id == RID_MOUSE)
        //{
        //	for (int i = 0; i<reqlen; i++) {	
        //		buffer[i] = 0x0f;
        //	}
        //}
      }
    return reqlen;
  }
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  Serial.print("rep ID: ");
  Serial.println(report_id);
  //for (int i = 0; i<bufsize;i++)
  //{
  //	Serial.print((char)buffer[i]);
  //}
  switch (report_type)
  {
    case HID_REPORT_TYPE_INVALID:
    case HID_REPORT_TYPE_INPUT:
    case HID_REPORT_TYPE_OUTPUT:
    break;

    case HID_REPORT_TYPE_FEATURE:
      {
        Serial.print("set_report type: ");
        Serial.println((int)report_type);
        if (report_type == HID_REPORT_TYPE_FEATURE && report_id == 1) {
          
        }
        if (report_type == HID_REPORT_TYPE_FEATURE && report_id == 2) {
          
        }
        //if (report_type == HID_REPORT_TYPE_FEATURE && report_id == RID_KEYBOARD) {
        //	
        //}
      }
    break;
  }
  //usb.sendReport(report_id,buffer,bufsize);

}

