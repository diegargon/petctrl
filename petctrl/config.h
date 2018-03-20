#ifndef CONFIG_H
#define CONFIG_H

#define CFG_BUFSIZE JSON_OBJECT_SIZE(8) + 256;
#include <ArduinoJson.h>
#include "MyFS.h"

struct ConfigData {
  char AP_SSID[32];
  char DogChip[32];
  char PhoneNumber[12];
  char STAEnabled[2];
  char STA_SSID[32];
  char STA_PASSWD[32];
  char OtaURL[64];
};

ConfigData Config;

bool loadConfig() {
  String ConfigFile;

  if (!readlineFromFile("config.dat", ConfigFile) ) {
    Serial.println("Error reading config file");
    return false;
  }
  const size_t capacity = CFG_BUFSIZE;

  StaticJsonBuffer<capacity> jsonBuffer;

  JsonObject &root = jsonBuffer.parseObject(ConfigFile);

  strlcpy(Config.AP_SSID, root["AP_SSID"], sizeof(Config.AP_SSID));
  strlcpy(Config.DogChip, root["DogChip"], sizeof(Config.DogChip));
  strlcpy(Config.PhoneNumber, root["PhoneNumber"], sizeof(Config.PhoneNumber));
  strlcpy(Config.STA_SSID, root["STA_SSID"], sizeof(Config.STA_SSID));
  strlcpy(Config.STA_PASSWD, root["STA_PASSWD"], sizeof(Config.STA_PASSWD));
  strlcpy(Config.OtaURL, root["OtaURL"], sizeof(Config.OtaURL));

  return true;
}

bool saveConfig() {
  String ConfigFile;

  const size_t capacity = CFG_BUFSIZE;
  StaticJsonBuffer<capacity> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  root["AP_SSID"] = Config.AP_SSID;
  root["DogChip"] = Config.DogChip;
  root["PhoneNumber"] = Config.PhoneNumber;
  root["STA_SSID"] = Config.STA_SSID;
  root["STA_PASSWD"] = Config.STA_PASSWD;
  root["OtaURL"] = Config.OtaURL;
  root.printTo(ConfigFile);
  //Serial.println(ConfigFile);
  savelineToFile("config.dat", ConfigFile);
}

#endif
