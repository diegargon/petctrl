
#ifndef MYWEBSERVER_H
#define MYWEBSERVER_H

#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#include "main_html.h"
#include "MyFS.h"

void loadMainPage() {
  String MAIN_public_page_tmp = MAIN_public_page;

  MAIN_public_page_tmp.replace("*ap ssid*", Config.AP_SSID);
  MAIN_public_page_tmp.replace("*cfg telefono*", Config.PhoneNumber);
  MAIN_public_page_tmp.replace("*cfg chip*", Config.DogChip);
  server.send( 200, "text/html", MAIN_public_page_tmp);

}

void loadSettingsPage() {

  String SettingsPageHTML_tmp = SettingsPageHTML;

  SettingsPageHTML_tmp.replace("*ap name*", Config.STA_SSID);
  SettingsPageHTML_tmp.replace("*ap pass*", Config.STA_PASSWD);
  SettingsPageHTML_tmp.replace("*pet name*", Config.AP_SSID);
  SettingsPageHTML_tmp.replace("*pet chip*", Config.DogChip);
  SettingsPageHTML_tmp.replace("*telefono*", Config.PhoneNumber);
  //SettingsPageHTML.replace("*OtaURL*", Config.OtaURL);
  server.send ( 200, "text/html", SettingsPageHTML_tmp);

}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void printServerArgs() {
  String args_data = "Server args:\n\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    args_data += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  Serial.println(args_data);
}

void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  Serial.println("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}

void handleFileUpload() { // upload a new file to the SPIFFS
  Serial.println("handlefileupload called");
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {                                   // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location", "/sucess");     // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

void handlePhotoUpload() {
  Serial.println("handlephotoupload called");

  HTTPUpload& upload = server.upload();

  Serial.println("Upload status: " + upload.status);
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "foto.jpg";
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("handlePhotoUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {                                   // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.println("handlePhotoUpload Size: "); Serial.println(upload.totalSize);
      server.send(200);
      //server.sendHeader("Location", "/sucess");     // Redirect the client to the success page
      //server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file ->" + upload.status);
    }
  }
}

bool loadFromSpiffs(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.htm";

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}

void handleWebRequests() {
  if (loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  Serial.println ("NotFound");
}

String getInstrumentsState() {
  String response_string;
  response_string = "\"lights\":\"";
  response_string += digitalRead(LIGHT_PIN);
  response_string += "\",";
  response_string += "\"sound\":\"";
  response_string += digitalRead(SOUND_PIN);
  response_string += "\",";
  response_string += "\"vibration\":\"";
  response_string += digitalRead(VIBRATION_PIN);
  response_string += "\"";
  return response_string;
}

void SetupMyWebServ() {
  //SERVER.ON
  server.on ( "/", []() {
    Serial.println("Loading /");
    Serial.print("Server args: ");
    Serial.print(server.argName(0) + ":");
    Serial.println(server.arg(0));
    loadMainPage();
  } );

  server.on ( "/associate", HTTP_POST, []() {

    String json_fail = "{\"status\":\"fail\"}";
    String json_success = "{\"status\":\"success\"}";

    Serial.println("Associating");

    //    const size_t capacity = CFG_BUFSIZE JSON_OBJECT_SIZE(8) + 256;
    //  StaticJsonBuffer<capacity> jsonBuffer;
    printServerArgs();
    String ap_name = server.arg("ap_name");
    String admin_password = server.arg("admin_password");
    ap_name.trim();
    admin_password.trim();
    if (ap_name.length() > 3 && admin_password.length() > 6) {
      //strcpy(Config.STA_SSID, ap_name.c_str());
      //strcpy(Config.STA_PASSWD, admin_password.c_str());
      //saveConfig();
      server.send(200, "application/json", json_success);
    } else {
      server.send(200, "application/json", json_fail);
    }

  });

  server.on ( "/client_info", HTTP_POST, []() {
    //printServerArgs();
    if (server.arg("admin_password") == Config.STA_PASSWD) {
      String json = "{\"status\":\"ok\",";
      json += "\"name\":\"" + String(Config.AP_SSID) + "\"";
      json += ",";
      json += "\"chipid\":\"" + String(Config.DogChip) + "\"";
      json += ",";
      json += "\"phone\":\"" + String(Config.PhoneNumber) + "\"";
      json += ",";
      json += "\"RSSI\":\"";
      json += WiFi.RSSI();
      json += "\"";
      json += ",";
      json += getInstrumentsState();
      json += "}";
      server.send(200, "aplication/json", json);
    } else {
      server.send(200, "aplication/json", "{\"status\":\"fail\",\"msg\":\"Incorrect password\"}");
    }
  });
  /*
    server.onNotFound ( [](){
    handleNotFound();
    });
  */
  server.onNotFound(handleWebRequests);

  server.on ( "/favicon.ico",   []() {
    Serial.println("favicon.ico");
    server.send( 200, "text/html", "" );
  });

  server.on ( "/settings", HTTP_GET, []() {
    Serial.println("Loading page settings");
    loadConfig();
    loadSettingsPage();

  } );

  server.on("/settings_android", HTTP_POST, []() {
    String json_fail = "{\"status\":\"fail\"}";
    String json_success = "{\"status\":\"success\"}";
    bool SomethingChange = false;
    Serial.println("Save settings android");

    printServerArgs();
    String petName  = server.arg("petName");
    String petChip  = server.arg("petChip");
    String telefono = server.arg("telefono");
    petName.trim();
    petChip.trim();
    telefono.trim();

    if (String(Config.AP_SSID) != petName) {
      strcpy(Config.AP_SSID, petName.c_str());
      SomethingChange = true;
    }
    if (String(Config.DogChip) != petChip) {
      strcpy(Config.DogChip, petChip.c_str());
      SomethingChange = true;
    }
    if (String(Config.PhoneNumber) != telefono) {
      strcpy(Config.PhoneNumber, telefono.c_str());
      SomethingChange = true;
    }

    if (SomethingChange) {
      Serial.println("Something change saving...");
      saveConfig();
      server.send(200, "application/json", json_success);
    } else {
      Serial.println("Nothing change, do nothing...");
      server.send(200, "application/json", json_fail);
    }

  });

  server.on("/settings", HTTP_POST, []() {
    printServerArgs();

    if ( (server.arg("restart") == "Reiniciar") || (server.arg("restart") == "1") ) {
      server.send(200, "aplication/json", "{\"status\":\"success\"}");
      delay(2000);
      ESP.restart();
    }
    if ( server.arg("turnoff") == "1") {
      //TODO: SHUTDOWN
    }
    if ( server.arg("save") == "Guardar" )
    {

      bool SomethingChange = false;

      String staName = server.arg("APName");
      String staPass = server.arg("APPasswd");
      String petName  = server.arg("petName");
      String petChip  = server.arg("petChip");
      String telefono = server.arg("telefono");
      String otaUrl   = server.arg("OtaURL");
      staName.trim();
      staPass.trim();
      petName.trim();
      petChip.trim();
      telefono.trim();
      otaUrl.trim();

      if (String(Config.AP_SSID) != petName) {
        strcpy(Config.AP_SSID, petName.c_str());
        SomethingChange = true;
      }
      if (String(Config.DogChip) != petChip) {
        strcpy(Config.DogChip, petChip.c_str());
        SomethingChange = true;
      }
      if (String(Config.PhoneNumber) != telefono) {
        strcpy(Config.PhoneNumber, telefono.c_str());
        SomethingChange = true;
      }
      if (String(Config.STA_SSID) !=  staName) {
        strcpy(Config.STA_SSID, staName.c_str());
        SomethingChange = true;
      }
      if (String(Config.STA_PASSWD) != staPass) {
        strcpy(Config.STA_PASSWD, staPass.c_str());
        SomethingChange = true;
      }
      if ( (String(Config.OtaURL) != otaUrl ) && (otaUrl != String("")) ) {
        strcpy(Config.OtaURL, otaUrl.c_str());
        SomethingChange = true;
      }

      if (SomethingChange) {
        Serial.println("Something change saving...");
        saveConfig();
      } else {
        Serial.println("Nothing change, do nothing...");
      }
      loadSettingsPage();
    }
  });

  server.on("/gpio_toggle", HTTP_POST, []() {
    String response;
    String msg;
    if (server.arg("toggle") == "lights") {
      digitalWrite(LIGHT_PIN, !digitalRead(LIGHT_PIN));
      msg = "\"lights toggle\"";
    }
    if (server.arg("toggle") == "sound") {
      digitalWrite(SOUND_PIN, !digitalRead(SOUND_PIN));
      msg = "\"sound toggle\"";
    }
    if (server.arg("toggle") == "vibration") {
      digitalWrite(VIBRATION_PIN, !digitalRead(VIBRATION_PIN));
      msg = "\"vibration toggle\"";
    }

    response = "{\"status\":\"success\",";
    response += "\"msg\":";
    response += msg;
    response += ",";
    response += getInstrumentsState();
    response += "}";

    server.send(200, "aplication/json", response);
  });

  server.on("/gpio_state", HTTP_POST, []() {

    String response_string;

    response_string = "{\"status\":\"success\"";
    response_string += ",";
    response_string += getInstrumentsState();
    response_string += "}";

    server.send(200, "aplication/json", response_string );

  });

  server.on("/photoupload", HTTP_POST, []() {
    //server.send(200, "text/plain", "");
    server.send(200);
  }, handlePhotoUpload
           );

  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);

  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //END SERVER_ON

  server.begin();
  Serial.println("HTTP server started");

}

#endif
