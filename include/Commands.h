#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>
enum commands {

 SetConfig  = 0B0000000000000010, 
 GetConfig  = 0B0000000000000011, 
 SetLed     = 0B0000000000000100,
 GetLed     = 0B0000000000000101,
 SetAxis    = 0B0000000000001000,
 GetAxis    = 0B0000000000001001,
 SetButton  = 0B0000000000010000,
 GetButton  = 0B0000000000010001,
 //Set  = 0B0000000000100000,
 //Get  = 0B0000000000100001,
 //Set  = 0B0000000001000000,
 //Get  = 0B0000000001000001,
 //Set  = 0B0000000010000000,
 //Get  = 0B0000000010000001,

 //Set  = 0B0000000100000000,
 //Get  = 0B0000000100000001,
 //Set  = 0B0000001000000000,
 //Get  = 0B0000001000000001,
 //Set  = 0B0000010000000000,
 //Get  = 0B0000010000000001,
 //Set  = 0B0000100000000000,
 //Get  = 0B0000100000000001,
 //Set  = 0B0001000000000000,
 //Get  = 0B0001000000000001,
 //Set  = 0B0010000000000000,
 //Get  = 0B0010000000000001,
 //Set  = 0B0100000000000000,
 //Get  = 0B0100000000000001,
 //Set  = 0B1000000000000000,
 //Get  = 0B1000000000000001,
//               ||||||||||||||||
//               /|||||||||||||||
//                /||||||||||||||
//                 /|||||||||||||
//                  /||||||||||||
//                   /|||||||||||
//                    /||||||||||
//                     /|||||||||
//                      /||||||||
//                       /|||||||
//                        /||||||
//                         /|||||
// BUTTON BIT --------------/||||
// AXIS BIT------------------/|||
// LED BIT -------------------/||
// CONFIG BIT -----------------/|
// SET/GET BIT -----------------/
  UpdateFirmware = 0B1111111111111111 // ONLY USED FOR MODIFYING THE FIRMWARE
};
enum ConfigSubCommands {
  GetDigital = 0B00000001,
  GetAnalog  = 0B00000010
};

enum AxisInputs {
  X,
  Y,
  Z,
  Rx,
  Ry,
  Rz,
  S,
  D,
  W,  
};

enum Buttons {
  Button1 = 1,
  Button2 ,
  Button3 ,
  Button4 ,
  Button5 ,
  Button6 ,
  Button7 ,
  Button8 ,
  Button9 ,
  Button10 ,
  Button11 ,
  Button12 ,
  Button13 ,
  Button14 ,
  Button15 ,
  Button16 ,
  Button17 ,
  Button18 ,
  Button19 ,
  Button20 ,
  Button21 ,
  Button22 ,
  Button23 ,
  Button24 ,
  Button25 ,
  Button26 ,

};



struct __attribute__((packed, aligned(1))) Input_Config {
  uint8_t input_type;
  uint8_t input_id;
  char input_name;
};
