#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

enum commandType {
  Config       = 0B00000010, 
  //Config    = 0B00000011, 
  SetLed       = 0B00000100,
  GetLed       = 0B00000101,
  SetAxis      = 0B00001000,
  GetAxis      = 0B00001001,
  SetButton    = 0B00010000,
  GetButton    = 0B00010001,
  SetMuxId     = 0B00100000,
  GetMuxId     = 0B00100001,
  //Set        = 0B01000000,
  //Get        = 0B01000001,
  //Set        = 0B10000000,
  //Get        = 0B10000001,
};

enum ConfigCommands {
  SetI2cId            = 0B0000000000000010,
  GetI2cId            = 0B0000000000000011,
  SetChipId           = 0B0000000000000100,// meant for testing only
  GetChipId           = 0B0000000000000101,
  SetWS2812LedPin     = 0B0000000000001000,
  GetWS2812LedPin     = 0B0000000000001001,
  SetAnalogResolution = 0B0000000000010000,
  GetAnalogResolution = 0B0000000000010001,
  SetButtonPin        = 0B0000000000100000,
  GetButtonPin        = 0B0000000000100001,
  SetAxisPin          = 0B0000000001000000,
  GetAxisPin          = 0B0000000001000001,
  SetHatPins          = 0B0000000010000000,
  GetHatPins          = 0B0000000010000001,
  SetMuxPins          = 0B0000000100000000,
  GetMuxPins          = 0B0000000100000001,
  SetButtonCount      = 0B0000001000000000,
  GetButtonCount      = 0B0000001000000001,
  SetHatCount         = 0B0000010000000000,
  GetHatCount         = 0B0000010000000001,
  SetAxisCount        = 0B0000100000000000,
  GetAxisCount        = 0B0000100000000001,
  SetMuxCount         = 0B0001000000000000,
  GetMuxCount         = 0B0001000000000001,
  SetDigitalPinMode   = 0B0010000000000000,
  GetDigitalPinMode   = 0B0010000000000001,
  SetAnalogPinMode    = 0B0100000000000000,
  GetAnalogPinMode    = 0B0100000000000001,
  SetInvert           = 0B1000000000000000,
  GetInvert           = 0B1000000000000001,
  // ONLY USED FOR MODIFYING THE FIRMWARE SETTINGS
  // WILL FORCE A REBOOT AFTERWARDS
  ConfigUpdate        = 0B1111111111111111 
};


