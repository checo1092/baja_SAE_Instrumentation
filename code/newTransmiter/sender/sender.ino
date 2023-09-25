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
  delay(100);
ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "12 23");
Serial.println(rs.getResponseDescription());

}

