/* This minimal example shows how to get single-shot range
measurements from the VL6180X.

The range readings are in units of mm. */

#include <Wire.h>
#include <VL6180X.h>

VL6180X sensor1;
VL6180X sensor2;
VL6180X sensor3;
VL6180X sensor4;

#define SHT_DIST1 2
#define SHT_DIST2 4
#define SHT_DIST3 16
#define SHT_DIST4 17

void setup() 
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(SHT_DIST1, OUTPUT);
  pinMode(SHT_DIST2, OUTPUT);
  pinMode(SHT_DIST3, OUTPUT);
  pinMode(SHT_DIST4, OUTPUT);

  digitalWrite(SHT_DIST1,LOW);
  digitalWrite(SHT_DIST2,LOW);
  digitalWrite(SHT_DIST3,LOW);
  digitalWrite(SHT_DIST4,LOW);

  delay(10);
  Serial.println("Set sensor 1");
  setsensor(sensor1,SHT_DIST1);

  Serial.println("Set sensor 2");
  setsensor(sensor2,SHT_DIST2);
  //Serial.println("Set sensor 3");
  //setsensor(sensor3,SHT_DIST3);
  //Serial.println("Set sensor 4");
  //setsensor(sensor4,SHT_DIST4);
  Serial.println("All sensors init");

  delay(500);
}

void setsensor(VL6180X &sensor, int SHT_pin)
{
  digitalWrite(SHT_pin,HIGH);
  delay(10);
  
  sensor.init();
  //sensor.writeReg(VL6180X::SYSRANGE__VHV_RECALIBRATE, 0);
  //sensor.writeReg(VL6180X::SYSRANGE__VHV_REPEAT_RATE,255);
  sensor.configureDefault();

  sensor.writeReg(VL6180X::SYSRANGE__THRESH_HIGH,255);
  sensor.writeReg(VL6180X::SYSRANGE__THRESH_LOW,0);
  sensor.writeReg(VL6180X::SYSRANGE__INTERMEASUREMENT_PERIOD,0); // Only for continous mode
  sensor.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
  sensor.writeReg16Bit(VL6180X::SYSRANGE__CROSSTALK_COMPENSATION_RATE,0);
  sensor.writeReg(VL6180X::SYSRANGE__CROSSTALK_VALID_HEIGHT,20); // default 20
  sensor.writeReg(VL6180X::SYSRANGE__EARLY_CONVERGENCE_ESTIMATE,0);

  sensor.writeReg(VL6180X::SYSRANGE__PART_TO_PART_RANGE_OFFSET, 0);
  sensor.writeReg(VL6180X::SYSRANGE__RANGE_IGNORE_VALID_HEIGHT, 0);
  sensor.writeReg16Bit(VL6180X::SYSRANGE__RANGE_IGNORE_THRESHOLD,0);

  sensor.writeReg(VL6180X::READOUT__AVERAGING_SAMPLE_PERIOD, 96);


  sensor.setScaling(1);


  sensor.writeReg(VL6180X::SYSRANGE__RANGE_IGNORE_THRESHOLD,0);

  //while(true)
  {

  }
}

double time1 = 0.0;
double time2 = 0.0;
double f = 0.0;
uint8_t md[] = {0,0,0,0};

void loop() 
{ 
  //Serial.println("Looping and printing");
  time1 = micros();
  md[1] = sensor1.readRangeSingleMillimeters();
  md[2] = sensor2.readRangeSingleMillimeters();
  //md[3] = sensor3.readRangeSingleMillimeters();
  //md[4] = sensor4.readRangeSingleMillimeters();
  time2 = micros();
  f = 1000000/(time2-time1);

  //Serial.println("Read values");

  //if (sensor1.timeoutOccurred()) { Serial.print(" 1:TIMEOUT"); }
  //if (sensor2.timeoutOccurred()) { Serial.print(" 2:TIMEOUT"); }
  //if (sensor3.timeoutOccurred()) { Serial.print(" 3:TIMEOUT"); }
  //if (sensor4.timeoutOccurred()) { Serial.print(" 4:TIMEOUT"); }

  Serial.print(md[1],DEC);
  Serial.print(",");
  Serial.print(md[2],DEC);
  Serial.print(",");
  // Serial.print(md[3],DEC);
  // Serial.print(",");
  //Serial.print(md[4],DEC);
  Serial.print(", f: ");
  Serial.print(f);
  
  delay(500);
  Serial.println();
}
