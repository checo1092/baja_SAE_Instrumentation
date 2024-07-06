#include "Wire.h"

const byte I2C_SLAVE_ADDR = 0x20;

void setup()
{
  Serial.begin(115200);

  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

long data = 0;
long response = 200;

long time1 = 0;
long time2 = 0;

void receiveEvent(int bytes)
{
  time1 = micros();
  data = 0;
  uint8_t index = 0;
  while (Wire.available())
  {
    byte* pointer = (byte*)&data;
    *(pointer + index) = (byte)Wire.read();
    index++;
  }
  time2 = micros();
}

void requestEvent()
{
  Wire.write((byte*)&response, sizeof(response));
}

double f = 0;

void loop() {

  if (data != 0)
  {
    Serial.print(data);
    f = 1000000.0/(time2-time1);
    Serial.print(",");
    Serial.print(time1);
    Serial.print(",");
    Serial.print(time2);
    Serial.print(",");
    Serial.println(f);
    data = 0;
  }
}