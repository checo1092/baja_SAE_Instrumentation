#include "Wire.h"
#include <ArduinoJson.h>

String json;
StaticJsonDocument<300> doc;

const char* text;
int id;
bool stat;
float value;

const byte I2C_SLAVE_ADDR = 0x20;

long data = 100;
long response = 0;
int requestIndex;

void SerializeObject()
{
    doc["text"] = "myText";
    doc["id"] = 10;
    doc["status"] = true;
    doc["value"] = 3.14;
 
    serializeJson(doc, json);
}

void setup()
{
  SerializeObject();
  Serial.begin(115200);
  Wire.begin();
  Serial.print("json: ");
  Serial.println(json);
}

void loop()
{
  sendData();
  delay(25);
}

void sendData()
{
  // Serial.println("Send data");

  // Serial.print("json.length()/32+1: ");
  // Serial.println(json.length()/32+1);

  for(requestIndex = 0; requestIndex < json.length()/32+1; requestIndex++ )
  {
    int ind_1 = requestIndex*32;
    int ind_2 = (requestIndex+1)*32;
    int num_chars;

    if((requestIndex+1)*32-1 > json.length())
    {
      ind_2 = json.length();
    }

    num_chars = ind_2 - ind_1;

    uint8_t json_subint[num_chars];
    const uint8_t* json_constsubint = &json_subint[0];

    Serial.print("ind_1: ");
    Serial.print(ind_1);
    Serial.print(", ind_2: ");
    Serial.println(ind_2);


    // Serial.println("json_subint: ");
    for(int i = 0;i < num_chars;i++)
    {
      json_subint[i] = (uint8_t)(json.charAt(ind_1+i));
      Serial.print(json.charAt(ind_1+i));
      Serial.print(" ");
      // Serial.println(json_subint[i]);
    }
    Serial.println();

    Serial.println();

    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(json_constsubint, num_chars);
    Wire.endTransmission();
    Wire.flush();
    delayMicroseconds(10);
  }

}