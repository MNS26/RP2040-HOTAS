#include "iniConfig-old.h"




IniConfig::IniConfig() {}

bool IniConfig::open(FIL* fil, const char* const filepath, uint8_t mode) {
  return f_open(fil, filepath, mode) == FR_OK ? true : false;
}

bool IniConfig::close(FIL* fil) {
  return f_close(fil) == FR_OK ? true : false;
}

std::string IniConfig::config()
{
    return str_config;
}

std::string IniConfig::readFile(FIL *fil)
{
    uint br; // dont really care
    assert(fil);

    str_config = "";
    str_config.reserve(f_size(fil));
    uint8_t buf[256];

    while (!f_eof(fil))
    {
        f_read(fil, buf, sizeof(buf), &br);
        str_config.append((char *)buf, br);
    }
    return str_config;
}

bool IniConfig::writeFile(FIL* fil) {
  uint bw;
  assert(fil);
//  while (!f_eof(fil))
//  {
//    f_read(fil,buf,sizeof(buf),&bw);
//    fileContent.append((char*)buf,bw);
//  }
//  int sectionStart, sectionEnd;
//  if (!findSection(fileContent, section, sectionStart, sectionEnd)) {
//    fileContent += "\n[" + std::string(section) + "]\n" + std::string(key) + "=" + std::string(value) + "\n";
//  } else {
//    std::string sectionContent = fileContent.substr(sectionStart, sectionEnd);
//    int keyStart, keyEnd;
//    if (!findKey(sectionContent, key, keyStart, keyEnd)) {
//      sectionContent += std::string(key) + "=" + std::string(value) + "\n";
//    } else {
//      sectionContent = sectionContent.substr(0, keyStart + strlen(key) + 1) + std::string(value) + sectionContent.substr(keyEnd);
//    }
//    fileContent = fileContent.substr(0, sectionStart) + sectionContent + fileContent.substr(sectionEnd);
//  }

  bw = 0;
  f_lseek(fil,0);
  return f_write(fil,str_config.c_str(),str_config.length(),&bw) == FR_OK ? true : false;
}

std::string IniConfig::getSection( const char *section) {
  int sectionStart, sectionEnd;
  sectionStart = 0;
  sectionEnd = 0;
  if (!findSection(str_config,section,sectionStart,sectionEnd)) {
    return "";
  }
  return str_config.substr(sectionStart, sectionEnd);
}

std::string IniConfig::getKey( const char *section, const char *key) {
  std::string sectionContent = getSection(section);
  int keyStart, keyEnd;
  keyStart = 0;
  keyEnd = 0;
  if (!findKey(sectionContent,key,keyStart,keyEnd)) {
    return "";
  }
  return sectionContent.substr(keyStart+strlen(key)+1, keyEnd);
}

bool IniConfig::addSection(const char* section) {
  int sectionStart, sectionEnd;
  if (!findSection(str_config, section, sectionStart, sectionEnd)) {
    str_config.append("\n[" + std::string(section) + "]\n");
    return true;
  }
  return false;
}

bool IniConfig::addKey(const char* section, const char* key) {
  int sectionStart, sectionEnd, keyStart, keyEnd;
  if (!findSection(str_config, section, sectionStart, sectionEnd)) {// return section position
    return false;
  }

  if (findKey(str_config.substr(sectionStart,sectionEnd), key, keyStart, keyEnd)) { // check if key is there or not
    return false;
  }

  str_config.insert(sectionEnd, std::string("\n") + key + std::string(" = "));
  return true;
}

bool IniConfig::setKey(const char* section, const char* key, const char* value) {
  int sectionStart, sectionEnd, keyStart, keyEnd;
  if (!findSection(str_config, section, sectionStart, sectionEnd) || !findKey(str_config.substr(sectionStart,sectionEnd), key, keyStart, keyEnd))
    return false;
  str_config.insert(keyEnd,value);
  return true;
}

bool IniConfig::removeKey(const char *section, const char *key) {
  int sectionStart, sectionEnd, keyStart, keyEnd;
  if (!findSection(str_config, section, sectionStart, sectionEnd) || !findKey(str_config.substr(sectionStart,sectionEnd), key, keyStart, keyEnd))
    return false;
  str_config.erase(sectionStart + keyStart, keyEnd - keyStart);
  return true;
}

bool IniConfig::removeSection(const char *section) {
  int sectionStart, sectionEnd;
  if (!findSection(str_config, section, sectionStart, sectionEnd))
    return false;
  str_config.erase(sectionStart, sectionEnd - sectionStart);
  return true;
}

std::string IniConfig::readString(const char *section, const char *key) { 
  return getKey(section,key);
}

bool IniConfig::readBool(const char *section, const char *key) { 
  return getKey(section,key)=="true" ? true:false;
}

int IniConfig::readInt(const char *section, const char *key) { 
  return std::stoi(getKey(section,key));
}

double IniConfig::readDouble(const char *section, const char *key) {
  return std::stod(getKey(section,key));
}

float IniConfig::readFloat(const char *section, const char *key) {
  return std::stof(getKey(section,key));
}

bool IniConfig::writeBool(const char *section, const char *key, const bool value) {
  return setKey(section, key, value ? "true" : "false");
}

bool IniConfig::writeInt( const char *section, const char *key, const int value) {
  return setKey(section, key, std::to_string(value).c_str());
}

bool IniConfig::writeFloat(const char *section, const char *key, const double value) {
  return setKey(section,key, std::to_string(value).c_str());
}

bool IniConfig::findSection(std::string& fileContent, const char* section, int& sectionStart, int& sectionEnd) {

  DBG_PRINTF("section: ");
  DBG_PRINTF(section);
  DBG_PRINTF("\n");

  int pos = 0;
  std::string lcSection =  toLowerCase(std::string(section));
  while (pos < fileContent.length()) {
    int lineEnd = fileContent.find('\n',pos);
    if (lineEnd == -1) 
      lineEnd = fileContent.length();
    std::string line = fileContent.substr(pos, lineEnd);
    line = trim(toLowerCase(line));

    if (line.length()>0 && line.rfind("[",0) && line.rfind("]",line.length())) {
      if (toLowerCase(line.substr(1, line.length() - 1)).compare(lcSection.c_str())) {
        sectionStart = pos;
        pos = lineEnd + 1;

        while (pos < fileContent.length() && (!fileContent.substr(pos, pos + 1).compare("[") || !fileContent.substr(pos, pos + 1).compare("\n"))) {
          pos = fileContent.find('\n', pos) + 1;
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

bool IniConfig::findKey(const std::string& sectionContent, const char* key, int& keyStart, int& keyEnd) {
  DBG_PRINTF("key: ");
  DBG_PRINTF(key);
  DBG_PRINTF("\n");
  int pos = 0;
  std::string lcKey = toLowerCase(std::string(key));

  while (pos < sectionContent.length()) {
    int lineEnd = sectionContent.find('\n', pos); // get end of line pos
    if (lineEnd == sectionContent.npos) 
      lineEnd = sectionContent.length();
    std::string line = sectionContent.substr(pos, lineEnd); // get current line
    line = trim(toLowerCase(line));

    if (line.rfind(lcKey,0) && line.at(lcKey.length()) == '=') {
      keyStart = pos;
      keyEnd = lineEnd;
      return true;
    }

    pos = lineEnd + 1;
  }
  return false;
}

std::string IniConfig::toLowerCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

std::string IniConfig::trim(const std::string & source) {
    std::string s(source);
    s.erase(0,s.find_first_not_of(" \n\r\t"));
    s.erase(s.find_last_not_of(" \n\r\t")+1);
    return s;
}