#ifndef IniConfig_h
#define IniConfig_h

#include <Arduino.h>

//so it behaves properly with any of them... (thx earle philhower for the example)
#if defined USE_SPIFFS
#include <FS.h>
#elif defined USE_LITTLEFS
#include <LittleFS.h>
#elif defined USE_SDFS
#include <SDFS.h>
#else
#error Please select a filesystem first by uncommenting one of the "-D USE_xxx" lines in the usb.ini file located in "config" folder.
#endif

class IniConfig {
public:
  IniConfig(FS* sd);
  bool file(const char* filepath);
  //void close();
  String read(const char* section, const char* key);
  bool write(const char* section, const char* key, const char* value);
  bool remove(const char* section, const char* key);
  bool removeSection(const char* section);

  bool readBool(const char* section, const char* key);
  int readInt(const char* section, const char* key);
  float readfloat(const char* section, const char* key);
  
  bool writeBool(const char* section, const char* key, const bool value);
  bool writeInt(const char* section, const char* key, const int value);
  bool writeFloat(const char* section, const char* key, const double value);
private:
    const char* _filepath;
    FS* _fileSystem;
    bool findSection(const char* section, String& fileContent, int& sectionStart, int& sectionEnd);
    bool findKey(const char* key, const String& sectionContent, int& keyStart, int& keyEnd);
};
#endif