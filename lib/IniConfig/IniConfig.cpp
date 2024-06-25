#include "IniConfig.h"

IniConfig::IniConfig(FS* sd) : _fileSystem(_fileSystem), _filepath(nullptr){}

// TODO: rework this one
bool IniConfig::open(const char *filepath) { 
  _filepath = filepath;
  return true;
}

String IniConfig::read(const char *section, const char *key) {
  File file = _fileSystem->open(_filepath, "r");
  if (!_file) {
    return String("");
  }
  String fileContent = _file.readString();
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
  File file = _fileSystem->open(_filepath, "rw");
  if (!_file) {
    return false;
  }
  String fileContent = file.readString();
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
  file.print(fileContent);
  file.close();
  return true;
}

bool IniConfig::remove(const char *section, const char *key) {
  File file = _fileSystem->open(_filepath, "rw");
  if (!_file) {
    return false;
  }
  String fileContent = file.readString();
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
  file.print(fileContent);
  file.close();
  return true;
}

bool IniConfig::removeSection(const char *section) {
  File file = _fileSystem->open(_filepath, "rw");
  if (!_file) {
    return false;
  }
  String fileContent = file.readString();
  int sectionStart, sectionEnd;
  if (!findSection(section, fileContent, sectionStart, sectionEnd)) {
    return false;
  }
  fileContent.remove(sectionStart, sectionEnd - sectionStart);
  file.print(fileContent);
  file.close();
  return true;
}

bool IniConfig::findSection(const char* section, String& fileContent, int& sectionStart, int& sectionEnd) {
  int pos = 0;
  while (pos < fileContent.length()) {
    int lineEnd = fileContent.indexOf('\n', pos);
    if (lineEnd == -1) lineEnd = fileContent.length();
    String line = fileContent.substring(pos, lineEnd);
    line.trim();
    if (line.startsWith("[") && line.endsWith("]")) {
      if (line.substring(1, line.length() - 1).equals(section)) {
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
  while (pos < sectionContent.length()) {
    int lineEnd = sectionContent.indexOf('\n', pos);
    if (lineEnd == -1) lineEnd = sectionContent.length();
    String line = sectionContent.substring(pos, lineEnd);
    line.trim();
    if (line.startsWith(key) && line.indexOf('=') != -1) {
      keyStart = pos;
      keyEnd = lineEnd;
      return true;
    }
    pos = lineEnd + 1;
  }
  return false;
 }
