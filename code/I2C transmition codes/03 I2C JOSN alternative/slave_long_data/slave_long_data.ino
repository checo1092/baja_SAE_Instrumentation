#include "Wire.h"

const byte I2C_SLAVE_ADDR = 0x20;

void setup()
{
  Serial.begin(115200);

  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

String data = "";
long response = 200;

long time1 = 1;
long time2 = 2;

void receiveEvent(int bytes)
{
  // data = "";
  uint8_t index = 0;
  while (Wire.available())
  {
    //byte* pointer = (byte*)&data;
    data += (char)Wire.read();
    index++;
  }
  Serial.print("Received ");
  Serial.print(index);
  Serial.println(" bytes");
}

void requestEvent()
{
  Wire.write((byte*)&response, sizeof(response));
}

double f = 0;
int lengthData = 52;

void loop() {

  if (data.endsWith("}"))
  {
    time2 = micros();
    Serial.print(data);
    f = 1000000.0/(time2-time1);
    Serial.print(",");
    Serial.print(time1);
    Serial.print(",");
    Serial.print(time2);
    Serial.print(",");
    Serial.println(f);
    data = "";
    time1 = micros();
  }
}