#include "IniConfig.h"

IniConfig::IniConfig(FS* sd) : _fileSystem(sd), _filepath(nullptr){}

// TODO: rework this one
void IniConfig::open(const char *filepath) { 
  _filepath = filepath;
}

String IniConfig::read(const char *section, const char *key) {
  assert(_fileSystem);
  File file = _fileSystem->open(_filepath,"r");
  if (!file) {
    return String("");
  }
  String fileContent = file.readString();
  file.close();
  int sectionStart, sectionEnd;
  if (!findSection(section, fileContent, sectionStart, sectionEnd)) {
    return String("");
  }
  String sectionContent = fileContent.substring(sectionStart, sectionEnd);
  int keyStart, keyEnd;
  if (!findKey(key, sectionContent, keyStart, keyEnd)) {
    return String("");
  }
  String value = sectionContent.substring(keyStart+strlen(key)+1, keyEnd);
  value.trim();
  return value;
}

bool IniConfig::write(const char *section, const char *key, const char *value) {
  assert(_fileSystem);
  File file = _fileSystem->open(_filepath,"r+");
  if (!file) {
    return false;
  }
  file.seek(0);
  String fileContent = file.readString();
  file.seek(0);
  int sectionStart, sectionEnd;
  if (!findSection(section, fileContent, sectionStart, sectionEnd)) {
    fileContent += "\n[" + String(section) + "]\n" + String(key) + "=" + String(value) + "\n";
  } else {
    String sectionContent = fileContent.substring(sectionStart, sectionEnd);
    int keyStart, keyEnd;
    if (!findKey(key, sectionContent, keyStart, keyEnd)) {
      sectionContent += String(key) + "=" + String(value) + "\n";
    } else {
      sectionContent = sectionContent.substring(0, keyStart + strlen(key) + 1) + String(value) + sectionContent.substring(keyEnd);
    }
    fileContent = fileContent.substring(0, sectionStart) + sectionContent + fileContent.substring(sectionEnd);
  }
  file.write(fileContent.c_str());
  file.close();
  return true;
}

bool IniConfig::remove(const char *section, const char *key) {
  assert(_fileSystem);
  File file = _fileSystem->open(_filepath,"r+");
  if (!file) {
    return false;
  }
  file.seek(0);
  String fileContent = file.readString();
  file.seek(0);
  int sectionStart, sectionEnd;
  if (!findSection(section, fileContent, sectionStart, sectionEnd)) {
    return false;
  }
  String sectionContent = fileContent.substring(sectionStart, sectionEnd);
  int keyStart, keyEnd;
  if (!findKey(key, sectionContent, keyStart, keyEnd)) {
    return false;
  }
  sectionContent.remove(keyStart, keyEnd - keyStart + 1);
  fileContent = fileContent.substring(0,sectionStart) + sectionContent + fileContent.substring(sectionEnd);
  file.write(fileContent.c_str());
  file.close();
  return true;
}

bool IniConfig::removeSection(const char *section) {
  assert(_fileSystem);
  File file = _fileSystem->open(_filepath,"r+");
  if (!file) {
    return false;
  }
  file.seek(0);
  String fileContent = file.readString();
  file.seek(0);
  int sectionStart, sectionEnd;
  if (!findSection(section, fileContent, sectionStart, sectionEnd)) {
    return false;
  }
  fileContent.remove(sectionStart, sectionEnd - sectionStart);
  file.write(fileContent.c_str());
  file.close();
  return true;
}

bool IniConfig::readBool(const char *section, const char *key) { 
  return read(section,key)=="true" ? true:false;
}

int IniConfig::readInt(const char *section, const char *key) { 
  return read(section,key).toInt();
}

float IniConfig::readFloat(const char *section, const char *key) {
  return read(section,key).toFloat();
}

bool IniConfig::writeBool(const char *section, const char *key,
                          const bool value) {
  return write(section, key, value ? "true" : "false");
}

bool IniConfig::writeInt(const char *section, const char *key,
                         const int value) {
  return write(section, key, String(value).c_str());
}

bool IniConfig::writeFloat(const char *section, const char *key,
                           const double value) {
  return write(section, key, String(value, 6).c_str());
}

bool IniConfig::findSection(const char* section, String& fileContent, int& sectionStart, int& sectionEnd) {
  int pos = 0;
  String lcSection = toLowerCase(String(section));
  while (pos < fileContent.length()) {
    int lineEnd = fileContent.indexOf('\n', pos);
    if (lineEnd == -1) lineEnd = fileContent.length();
    String line = fileContent.substring(pos, lineEnd);
    line.toLowerCase();
    line.trim();
    if (line.startsWith("[") && line.endsWith("]")) {
      if (toLowerCase(line.substring(1, line.length() - 1)).equals(lcSection)) {
        sectionStart = pos;
        pos = lineEnd + 1;
        while (pos < fileContent.length() && (!fileContent.substring(pos, pos + 1).equals("[") || !fileContent.substring(pos, pos + 1).equals("\n"))) {
          pos = fileContent.indexOf('\n', pos) + 1;
          if (pos == 0) break;
        }
        sectionEnd = pos;
        return true;
      }
    }
    pos = lineEnd + 1;
  }
  return false;
}

bool IniConfig::findKey(const char* key, const String& sectionContent, int& keyStart, int& keyEnd) {
  int pos = 0;
  String lcKey = toLowerCase(String(key));
  while (pos < sectionContent.length()) {
    int lineEnd = sectionContent.indexOf('\n', pos); // get end of line pos
    if (lineEnd == -1) lineEnd = sectionContent.length();
    String line = sectionContent.substring(pos, lineEnd); // get current line
    line.trim();
    line.toLowerCase();
    if (line.startsWith(lcKey) && line.charAt(strlen(lcKey.c_str())) == '=') {
      keyStart = pos;
      keyEnd = lineEnd;
      return true;
    }
    pos = lineEnd + 1;
  }
  return false;
}

String IniConfig::toLowerCase(const String& str) {
  String lowerStr = str;
  lowerStr.toLowerCase();
  return lowerStr;
}