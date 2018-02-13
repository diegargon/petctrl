#include "MyFS.h"
#include "config.h"
#include "MyWebServer.h"
#include "MyWifi.h"
#include "main_html.h"

void setup() {
  
	Serial.begin(115200);
  //Serial.begin(9600);
	Serial.println();  
  setupFS();
  if (loadConfig() ) {    
    SetupMyAP();
    SetupMyWebServ();
  }
 
}


void loop() {
  delay(1000);  
  server.handleClient( );

  
  if (needStaReconnect()) {
    delay(10000);
  }
 
//FIN
}



