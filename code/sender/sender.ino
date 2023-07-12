//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for GPS
//#include <TinyGPS.h>

#define GPS_BAUD_RATE               9600
#define GPS_RX_PIN                  34
#define GPS_TX_PIN                  12

//TinyGPS gps;

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
#define TXPOWER 17
//Supported values are between 6 and 12, default to 7
#define SPREDINGFACTOR 6
// true enables implicit header mode, false enables explicit header mode (default)
#define IMPLICIDHEADER true

//packet counter
int counter = 0;

void setup() {

  //Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  //initialize Serial Monitor
  Serial.begin(115200);

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  LoRa.setTxPower(TXPOWER);
  LoRa.setSpreadingFactor(SPREDINGFACTOR);
  LoRa.beginPacket(IMPLICIDHEADER);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  delay(2000);
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
  // bool newData = false;
  // unsigned long chars;
  // unsigned short sentences, failed;
  
  // // Intentar recibir secuencia durante un segundo
  // for (unsigned long start = millis(); millis() - start < 1000;)
  // {
  //   while (Serial1.available())
  //   {
  //     char c = Serial1.read();
  //     if (gps.encode(c)) // Nueva secuencia recibida
  //       newData = true;
  //   }
  // }

  // if (newData)
  // {
  //   float flat, flon;
  //   unsigned long age;
  //   gps.f_get_position(&flat, &flon, &age);
  //   Serial.print("LAT=");
  //   Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  //   Serial.print(" LON=");
  //   Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  //   Serial.print(" SAT=");
  //   Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
  //   Serial.print(" PREC=");
  //   Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  // }

  //gps.stats(&chars, &sentences, &failed);
  // Serial.print(" CHARS=");
  // Serial.print(chars);
  // Serial.print(" SENTENCES=");
  // Serial.print(sentences);
  // Serial.print(" CSUM ERR=");
  // Serial.println(failed);

  loraSend();
}

