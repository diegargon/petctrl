
#ifndef MYWIFI_H
#define MYWIFI_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "MyFS.h"

//FUNCTIONS
void StationConn(void);
void SetupMyAP(void);

String ssid;

String AP_STATION_SSID;
String AP_STATION_PASSWD;

IPAddress myIP;
IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

bool needStaReconnect() {
  //if (WIFI_STA_conn != WL_CONNECTED) {
  if (WiFi.localIP().toString() == "0.0.0.0") { //WORKAROUND
    return true;
  }
  return false;
}

void StationConn() {

  //CLIENT MODE
  AP_STATION_SSID = Config.STA_SSID;
  AP_STATION_PASSWD = Config.STA_PASSWD;
  WiFi.setAutoReconnect(false);
  WiFi.begin(AP_STATION_SSID.c_str(), AP_STATION_PASSWD.c_str());
  Serial.print("AP_SSID ");
  Serial.println(AP_STATION_SSID.c_str());
  delay(2000);
  if (!needStaReconnect()) {
    Serial.print("Local IP: ");
    Serial.print(WiFi.localIP());
    Serial.print(" RSSI: ");
    Serial.println(WiFi.RSSI());
  }
}

void SetupMyAP() {
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP_STA);
  //AP MODE
  ssid = String(Config.AP_SSID) + "-Pet"; // String(ESP.getChipId(),HEX);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid.c_str()); //WiFi.softAP(ssid, password);
  delay(100);
  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //STAT/CLIENT MODE
  if (Config.STAEnabled) {
    StationConn();
  }
}

#endif
