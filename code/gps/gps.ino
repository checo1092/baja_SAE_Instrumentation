#include <SPI.h>
#include <TinyGPS++.h>                       

TinyGPSPlus gps;                            
// HardwareSerial Serial1(1); 

int pktCount = 0; // count how many packets sent

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 34, 12);   //17-TX 18-RX for GPS
}

void print_info_json(){
  Serial.print("{\'Valid\': \'");
  Serial.print(gps.location.isValid());
  Serial.print("\', \'Lat\': \'");
  Serial.print(gps.location.lat(), 5);
  Serial.print("\', \'Long\': \'");
  Serial.print(gps.location.lng(), 4);
  Serial.print("\', \'Satellites\': \'");
  Serial.print(gps.satellites.value());
  Serial.print("\', \'Altitude\': \'");
  Serial.print(gps.altitude.feet());
  Serial.print("\', \'Time\': \'");
  Serial.printf("%.2d:%.2d:%.2d",gps.time.hour(),gps.time.minute(),gps.time.second());
  Serial.println("\'}");
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

void loop()
{
  //print_info_json();
  print_gps();

  smartDelay(1000);                                      

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}




