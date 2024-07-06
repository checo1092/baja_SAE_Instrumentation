#include "Wire.h"

const byte I2C_SLAVE_ADDR = 0x20;

long data = 100;
long response = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
}

void loop()
{
  sendToSlave();
  requestToSlave();
  delay(2000);
}

void sendToSlave()
{
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write((byte*)&data, sizeof(data));
  Wire.endTransmission();
}

void requestToSlave()
{
  response = 0;
  Wire.requestFrom(I2C_SLAVE_ADDR, sizeof(response));

  uint8_t index = 0;
  byte* pointer = (byte*)&response;
  while (Wire.available())
  {
    *(pointer + index) = (byte)Wire.read();
    index++;
  }

  Serial.println(response);
}