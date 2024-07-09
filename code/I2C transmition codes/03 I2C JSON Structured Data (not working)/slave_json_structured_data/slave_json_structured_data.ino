#include "Wire.h"
#include <ArduinoJson.h>
 
String json;
StaticJsonDocument<300> doc;

const byte I2C_SLAVE_ADDR = 0x20;

const char ASK_FOR_LENGTH = 'L';
const char ASK_FOR_DATA = 'D';

char request = ' ';
int requestIndex = 0;

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
  Serial.begin(115200);

  // json = String("5");
  // json += String("6");
  // json += String("7");
  SerializeObject();

  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  // Serial.print("json: ");
  // Serial.print(json);
  // Serial.print(", 0-31: ");
  // Serial.print(json.substring(0,32));
  // Serial.print(", 32-52");
  // Serial.println(json.substring(32,52));
  delay(5000);
}

void receiveEvent(int bytes)
{
  while (Wire.available())
  {
    request = (char)Wire.read();
  }
}

void requestEvent()
{
  Serial.print("request type: ");
  Serial.println(request);
  if(request == ASK_FOR_LENGTH)
  {
    requestDataLength();
    // Wire.write(json.length());
    // requestIndex = 0;
    Serial.print("Asked for length: ");
    Serial.println(json.length());
  }
  if(request == ASK_FOR_DATA)
  {
    requestData();
  }
}

void requestDataLength()
{
  unsigned int length[1] = {json.length()};
  int num_chars = sizeof(length)/sizeof(length[0]);
  Serial.print("num_chars: ");
  Serial.println(num_chars);

  uint8_t json_subint[num_chars];
  const uint8_t* json_constsubint = &json_subint[0];


  // Serial.print("json_subint: ");
  for(int i = 0;i < num_chars;i++)
  {
    int num = (length[0]/(int)pow(10.0,i*1.0))%10;
    json_subint[i] = (uint8_t)(num);
    Serial.println(json_subint[i]);
    Serial.print(" ");
    
  }
  Serial.println();

  // Serial.println();

  Wire.slaveWrite(json_constsubint,num_chars);
  // Wire.slaveWrite(json.length());
  // requestIndex = 0;
  // Serial.print("Request for length: ");
  // Serial.println(json.length());
  // Serial.println();
}

void requestData()
{
  Serial.println("Asked for data");

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

  Wire.slaveWrite(json_constsubint,num_chars);

  if(ind_2 < json.length())
  {
    requestIndex++;
  }
  else
  {
    requestIndex = 0;
  }
}

void loop() 
{
}