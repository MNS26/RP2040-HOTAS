#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>
enum commands {
 GET_CONFIG  = 0b0000000000000001, // IT HAS NO SET OPTION
 SET_LED     = 0b0000000000000010,
 GET_LED     = 0b0000000000000011,
 SET_AXIS    = 0b0000000000000100,
 GET_AXIS    = 0b0000000000000101,
 SET_BUTTON  = 0b0000000000001000,
 GET_BUTTON  = 0b0000000000001001,
 //SET_  = 0b0000000000010000,
 //GET_  = 0b0000000000010001,
 //SET_  = 0b0000000000100000,
 //GET_  = 0b0000000000100001,
 //SET_  = 0b0000000001000000,
 //GET_  = 0b0000000001000001,
 //SET_  = 0b0000000010000000,
 //GET_  = 0b0000000010000001,
 //SET_  = 0b0000000100000000,
 //GET_  = 0b0000000100000001,
 //SET_  = 0b0000001000000000,
 //GET_  = 0b0000001000000001,
 //SET_  = 0b0000010000000000,
 //GET_  = 0b0000010000000001,
 //SET_  = 0b0000100000000000,
 //GET_  = 0b0000100000000001,
 //SET_  = 0b0001000000000000,
 //GET_  = 0b0001000000000001,
 //SET_  = 0b0010000000000000,
 //GET_  = 0b0010000000000001,
 //SET_  = 0b0100000000000000,
 //GET_  = 0b0100000000000001,
 //SET_  = 0b1000000000000000,
 //GET_  = 0b1000000000000001,
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
//                          /||||
// BUTTON BIT ---------------/|||
// AXIS BIT-------------------/||
// LED BIT --------------------/|
// GET/SET BIT -----------------/
};

enum input_id {
  THROTTLE = 0
};

enum config_id {
  
};


struct __attribute__((packed, aligned(1))) Command{
  uint16_t command_type;
  uint16_t id;
  void* data;
};

struct __attribute__((packed, aligned(1))) Input_Config {
  uint8_t input_type;
  uint8_t input_id;
  char input_name;
};
