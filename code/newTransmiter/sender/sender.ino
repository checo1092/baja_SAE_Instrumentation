#include "Arduino.h"
#include "LoRa_E220.h"
//ADS1115
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;
//GPS NEO 8M
#include <TinyGPS++.h>
TinyGPSPlus gps;

// ---------- esp32 pins --------------
LoRa_E220 e220ttl(&Serial2, 25, 27, 26);  //  RX AUX M0 M1

//ADS const and vars
boolean ads_on = false;
//Sensor 1 psi =  382.92x - 186.64
const float m1 = 382.92;
const float b1 = 186.64;
//Sensor 2 psi =  364.89x - 160.89
const float m2 = 364.89;
const float b2 = 160.89;

int16_t adc0, adc1;
float psi_1 = -1, psi_2 = -1;
float lat = -1, lng = -1;
String data = "";

void setup() {
  //Serial.begin(9600);
  delay(500);
  // Startup all pins and UART
  e220ttl.begin();

  // ads.setGain(GAIN_TWOTHIRDS);  +/- 6.144V  1 bit = 0.1875mV (default)
  if (ads.begin()) {
    ads_on = true;
  }
  Serial1.begin(9600, SERIAL_8N1, 34, 12);  //17-TX 18-RX for GPS
  Serial.begin(9600, SERIAL_8N1);  //OpenLog
}

void loop() {
  float v1 = 0;
  float v2 = 0;


  data = "";
  //read the GPS data 
  if (Serial1.available()) {
    lat = gps.location.lat();
    lng = gps.location.lng();
  }
  //Read the voltage of the pressure sensors and convert it to PSI
  if (ads_on) {
    psi_1 = m1 * ads.computeVolts(ads.readADC_SingleEnded(0)) - b1;
    psi_2 = m2 * ads.computeVolts(ads.readADC_SingleEnded(1)) - b2;
    
    v1 = ads.computeVolts(ads.readADC_SingleEnded(0));
    v2 = ads.computeVolts(ads.readADC_SingleEnded(1));
  }

  //Concat the data
  data.concat(String(psi_1));
  data.concat(",");
  data.concat(String(psi_2));
  data.concat(",");
  data.concat(String(lat));
  data.concat(",");
  data.concat(String(lng));
  data.concat(",");
  data.concat(String(millis()));
  //data.concat("\n");

  //Send the data
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, data);
  //Serial.println(rs.getResponseDescription());

  Serial.println(data); 
  Serial1.println(data);


  delay(500);
}
