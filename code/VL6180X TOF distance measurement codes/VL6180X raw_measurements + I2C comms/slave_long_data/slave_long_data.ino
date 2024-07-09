#include "Wire.h"

const byte I2C_SLAVE_ADDR = 0x20;
const int num_VL6180 = 4;

unsigned char dist_data[num_VL6180] = {};

int ind_data = 0;

String data = "";
long response = 200;

long time1 = 1;
long time2 = 2;

void setup()
{
  Serial.begin(115200);

  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  // Wire.onRequest(requestEvent);
}


void receiveEvent(int bytes)
{
  // data = "";
  ind_data = 0;
  while (Wire.available())
  {
    //byte* pointer = (byte*)&data;
    // data += String((unsigned char)Wire.read());
    // data += ",";
    if(ind_data < num_VL6180)
    {
      dist_data[ind_data] = (unsigned char)Wire.read();
    }
    ind_data++;
  }
  // Serial.print("Received ");
  // Serial.print(ind_data);
  // Serial.println(" bytes");
}

double f = 0;
int lengthData = 52;

void ranges_SerialPrint()
{
  for(int i = 0; i < num_VL6180; i++)
  {
    //Serial.print("Sensor ");
    //Serial.print(i);
    //Serial.print(" :");
    Serial.print(dist_data[i]);

    // Serial.print("mm");
    if(i==num_VL6180) Serial.println();
    else Serial.print(",");
  }
  // Serial.print(sizeof(dist_data));
  // Serial.println(";");
}

double calculate_frec(long time1, long time2)
{
  f = (double)1000000.0/(time2-time1);
  // Serial.println();
  // Serial.print("Times: ");
  // Serial.print(time1,4);
  // Serial.print(",");
  // Serial.print(time2,4);
  // Serial.print(",");
  Serial.print("Frec: ");
  Serial.println(f,6);
  return f;
}

void loop() {

  if (ind_data > 0)
  {
    // Serial.print("ind_data: ");
    // Serial.println(ind_data);
    time2 = micros();
    ranges_SerialPrint();
    Serial.println(" ");
    // calculate_frec(time1, time2);
    // data = "";
    ind_data = 0;
    time1 = micros();
  }
}