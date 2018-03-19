#define ONBOARD_LED 2
#define LIGHT_PIN 3
#define SOUND_PIN 4
#define VIBRATION_PIN 5

#include "MyFS.h"
#include "config.h"
#include "MyWifi.h"
#include "MyWebServer.h"
#include "main_html.h"

int reconnectDelay = 3000;
unsigned long lastMillis;
 
void setup() {

  Serial.begin(115200);
  //Serial.begin(9600);

  Serial.println();

  lastMillis = millis();

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(SOUND_PIN, OUTPUT);
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(SOUND_PIN, LOW);
  digitalWrite(VIBRATION_PIN, LOW);

  setupFS();
  if (loadConfig() ) {
    SetupMyAP();
    SetupMyWebServ();
   StationConn();    
  }

}

void loop() {
  delay(1000);

  server.handleClient( );

  if (needStaReconnect()) {
    unsigned long nextCheck = lastMillis + reconnectDelay;
    if (millis() >  nextCheck) {    
      Serial.print("Trying StationConn...");
      lastMillis = millis();
      if (reconnectDelay < 15000) reconnectDelay += 1000;
       Serial.println(reconnectDelay);
      StationConn();      
    }
  } else if (reconnectDelay > 3000) {
    Serial.println("Connected: Reset reconnect to 5000");
    reconnectDelay = 3000;
  }

}



