#ifndef IniConfig_h
#define IniConfig_h

//#include <Arduino.h>
#include <stdint.h>
#include <string>
#include <string.h>
#include <cctype>  // for tolower
#include <stdbool.h>
#include <stdlib.h>
#include <iostream>

//==== SD CARD ====
#include "f_util.h"
#include "ff.h"
#include "hw_config.h"
//================



//so it behaves properly with any of them... (thx earle philhower for the example)
//#if defined USE_SPIFFS
///#include <FS.h>
//#elif defined USE_LITTLEFS
//#include <LittleFS.h>
//#elif defined USE_SDFS
//#include <SDFS.h>
//#else
//#error Please select a filesystem first by uncommenting one of the "-D USE_xxx" lines in the usb.ini file located in "config" folder.
//#endif

class IniConfig {
public:
  IniConfig();


  bool open(FIL *fil, const char *const filepath, uint8_t mode);

  bool close(FIL *fil);

  std::string config();

  std::string readFile(FIL *fil);
  bool writeFile(FIL *fil);

  std::string getSection(const char *section);
  std::string getKey(const char *section, const char *key);

  bool addSection(const char *section);
  bool addKey(const char *section, const char* key);

  bool setKey(const char* section, const char* key, const char* value);

  bool removeSection(const char* section);
  bool removeKey(const char* section, const char* key);

  std::string readString(const char *section, const char *key);
  bool readBool(const char* section, const char* key);
  int readInt(const char* section, const char* key);
  double readDouble( const char *section, const char *key);
  float readFloat(const char* section, const char* key);
  
  bool writeBool(const char* section, const char* key, const bool value);
  bool writeInt(const char* section, const char* key, const int value);
  bool writeFloat(const char* section, const char* key, const double value);

private:
  const char* _filepath;
  FRESULT fr;
  std::string str_config;

  bool findSection(std::string& fileContent, const char* section, int& sectionStart, int& sectionEnd);
  bool findKey(const std::string& sectionContent, const char* key, int& keyStart, int& keyEnd);

  std::string toLowerCase(const std::string& str);
  std::string trim(const std::string & source);
};
#endif