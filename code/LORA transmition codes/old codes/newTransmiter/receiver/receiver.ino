#include "Arduino.h"
#include "LoRa_E220.h"

// ---------- esp32 pins --------------
 LoRa_E220 e220ttl(&Serial2, 14, 17, 16);  //  RX AUX M0 M1

void setup() {
  Serial.begin(115200);
  delay(500);
  // Startup all pins and UART
  
  if(e220ttl.begin())
  {
    Serial.println("LoRa e220 is up");
  }
  else
  {
    Serial.println("LoRa e220 not set up");
  }
}

void loop() {
	// If something available
  if (e220ttl.available()>1) {

	  // read the String message
	ResponseContainer rc = e220ttl.receiveMessage();

		// Print the data received
		//Serial.println(rc.data);
    Serial.println(rc.data);

	}
  
}

