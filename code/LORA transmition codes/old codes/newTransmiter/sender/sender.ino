#include "Arduino.h"
//E220 lora module
#include "LoRa_E220.h"
//MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
Adafruit_MPU6050 mpu;
//ADS1115
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;
//GPS NEO 8M
//#include <SPI.h>
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

//MPU parameters
float ax_offset = -0.82;
float ay_offset = -0.28;
float az_offset = 0.20;
float gx_offset = 0.369;
float gy_offset = 0.129;
float gz_offset = 0.314;


int16_t contador = 0;

void setup() {
  //Serial.begin(9600);
  delay(500);
  // Startup all pins and UART
  e220ttl.begin();

  // ads.setGain(GAIN_TWOTHIRDS);  +/- 6.144V  1 bit = 0.1875mV (default)
  if (ads.begin()) {
    ads_on = true;
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  Serial.println("+-8G");
  mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);
  Serial.print("Filter bandwidth set to: ");
  Serial.println("10 Hz");

  Serial.begin(9600, SERIAL_8N1);           //OpenLog
  Serial1.begin(9600, SERIAL_8N1, 19, 18);  //17-TX 18-RX for GPS
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  
  
  }

String printMPU(sensors_event_t a, sensors_event_t g) {
  //Concat the data
  String mpuData = "";
  mpuData.concat(String(a.acceleration.x + ax_offset,3));
  mpuData.concat(",");
  mpuData.concat(String(a.acceleration.y + ay_offset,3));
  mpuData.concat(",");
  mpuData.concat(String(a.acceleration.z + az_offset,3));
  mpuData.concat(",");
  mpuData.concat(String(g.gyro.x + gx_offset,3));
  mpuData.concat(",");
  mpuData.concat(String(g.gyro.y + gy_offset,3));
  mpuData.concat(",");
  mpuData.concat(String(g.gyro.z + gz_offset,3));
  return mpuData;
}

String print_gps() {
  String gpsData = "";
  delay(500);
  if (Serial1.available()) {
    gps.encode(Serial1.read());
    if (gps.location.isValid()) {
      gpsData.concat(String(gps.location.lat(),8));
      gpsData.concat(",");
      gpsData.concat(String(gps.location.lng(),8));
    } else {
      gpsData.concat("-1 -1");
    }
  } else {
    gpsData.concat("-1 -1");
  }

  return gpsData;
}

void loop() {
  data = "";
  float v1 = 0;
  float v2 = 0;

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);


  //Read the voltage of the pressure sensors and convert it to PSI
  if (ads_on) {
    psi_1 = m1 * ads.computeVolts(ads.readADC_SingleEnded(0)) - b1;
    psi_2 = m2 * ads.computeVolts(ads.readADC_SingleEnded(1)) - b2;

    v1 = ads.computeVolts(ads.readADC_SingleEnded(0));
    v2 = ads.computeVolts(ads.readADC_SingleEnded(1));
  }

  //Concat the data
  data.concat(print_gps());
  data.concat(",");
  data.concat(printMPU(a, g));
  data.concat(",");
  data.concat(String(psi_1));
  data.concat(",");
  data.concat(String(psi_2));
  data.concat(",");
  data.concat(String(lat));
  data.concat(",");
  data.concat(String(lng));
  data.concat(",");
  data.concat(String(millis()));
  data.concat(",");
  data.concat(String(contador++));
  //data.concat("\n");

  //Send the data
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, data);
  //Serial.println(rs.getResponseDescription());

  Serial.println(data);  //print and save the data
  //Serial1.println(data);


  delay(500);
}
