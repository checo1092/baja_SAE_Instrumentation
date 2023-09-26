//Libraries for the LoRa module
#include <SPI.h>
#include <LoRa.h>

//Libraries for the GPS
#include <SPI.h>
#include <TinyGPS++.h>

//Parameters and configurations for the GPS
TinyGPSPlus gps;
//34-TX 12-RX
#define GPS_TX 34
#define GPS_RX 12

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26
// Parameters for the LoRa protocol
#define BAND 433E6
// Supported values are between 2 and 17, defualt to 14
#define TXPOWER 20
//#define TXPOWER 17
//Supported values are between 6 and 12, default to 7
#define SPREDINGFACTOR 8
// true enables implicit header mode, false enables explicit header mode (default)
#define IMPLICIDHEADER true

//Global variables
int pktCount = 0; // count how many packets sent

void setup(){
  // Serial communication
  Serial.begin(115200);
  
  // GPS Serial communication
  Serial1.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);   	
  
  // Lora serial communication
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  LoRa.setTxPower(TXPOWER);
  LoRa.setSpreadingFactor(SPREDINGFACTOR);
  LoRa.beginPacket(IMPLICIDHEADER);
  // Check if the Lora communication was stablished
  if (!LoRa.begin(BAND)) {
  	Serial.println("Starting LoRa failed!, rebooting now.");
  	ESP.restart();
  }
}

void print_gps(){
  if(gps.location.isValid()){
    Serial.print(gps.location.lat(), 5);
    Serial.print(" ");
    Serial.print(gps.location.lng(), 4);
    Serial.print(" ");
    Serial.println(gps.satellites.value());

  }else {
    Serial.println("-1 -1");
  }
}

void loraSend(){
      Serial.print("Sending packet: ");
      Serial.println(counter);

      //Send LoRa packet to receiver
      LoRa.beginPacket();
      LoRa.print("hello ");
      LoRa.print(counter);
      LoRa.endPacket();

      counter++;
      delay(1000);
}



void loop(){

  print_gps();

  smartDelay(1000);                                      

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do{
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}




