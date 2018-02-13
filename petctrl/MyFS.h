#ifndef MYFS_H
#define MYFS_H

#include <FS.h>

File fsUploadFile;

//format bytes

String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

void setupFS() {
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

bool readlineFromFile(String file, String &Content)
{  
  Serial.print("File " + file);
  File f = SPIFFS.open(String("/" + file), "r");
  if (!f) {    
    Serial.println(" open Failed!");
  }
  else {
    Content =  f.readStringUntil('\r');
    Content.replace("\n", "");
    Serial.println(" open succesfull");
    Serial.println("Content: " + Content);
    f.close();
    return true;
  }
  f.close();
    
  return false;
}

bool savelineToFile(String fileNameForSave, String &DataToSave)
{
  File f = SPIFFS.open(String("/" + fileNameForSave), "w");
  if (!f)
  {
    Serial.println("file open failed");
  }
  else
  {
    Serial.println("Saving config");
    Serial.println(DataToSave);
    f.println(String(DataToSave + String('\r')));
    f.close();
    return true;
  }
  f.close();
  return false;
}

/*
String readConfig(String key)
{
  String WhatIwillReturn;
  //SPIFFS.begin();
  File f = SPIFFS.open(String("/" + key + ".dat"), "r");
  if (!f)
  {
    Serial.print("file open failed  :");
    Serial.println(key);
  }
  else
  {
    WhatIwillReturn =  f.readStringUntil('\r');
    WhatIwillReturn.replace("\n", "");
    Serial.print("read file config:");
    Serial.println(WhatIwillReturn);
    f.close();
    return WhatIwillReturn;
  }
}
*/


/*
void SaveDataToFile(String fileNameForSave, String DataToSave)
{
  Serial.println(fileNameForSave);
  //SPIFFS.begin();
  File f = SPIFFS.open(String(" /data/" + fileNameForSave + ".dat"), "w");
  if (!f)
  {
    Serial.println("file open failed");
  }
  else
  {
    f.println(String(DataToSave + String('\r')));
    f.close();
  }
  return;
}
*/


#endif
