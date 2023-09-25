#include "Arduino.h"
#include "LoRa_E220.h"

// ---------- esp32 pins --------------
 LoRa_E220 e220ttl(&Serial2, 25, 27, 26);  //  RX AUX M0 M1

void setup() {
  Serial.begin(9600);
  delay(500);
  // Startup all pins and UART
  e220ttl.begin();
}

void loop() {
	// If something available
  if (e220ttl.available()>1) {

	  // read the String message
	ResponseContainer rc = e220ttl.receiveMessage();

		// Print the data received
		Serial.println(rc.data);
	}
  
}

