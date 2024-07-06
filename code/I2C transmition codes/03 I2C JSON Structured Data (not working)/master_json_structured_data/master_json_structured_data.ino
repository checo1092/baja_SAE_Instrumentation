#include "Wire.h"
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

const byte I2C_SLAVE_ADDR = 0x20;

String response = "";
StaticJsonDocument<300> doc;

const char* text;
int id;
bool stat;
float value;

const char ASK_FOR_LENGTH = 'L';
const char ASK_FOR_DATA = 'D';
unsigned int responseLength = 1;
unsigned int requested_bytes = 0;

unsigned long time1 = 0;
unsigned long time2 = 0;
double f = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  time1 = micros();
  askSlave();
  time2 = micros();
  if (response != "") DeserializeResponse();

  // f = 1000000.0/(time2-time1);

  // Serial.print("frec: ");
  // Serial.println(f);
  delay(2000);
}

void askSlave() {
  response = "";
  Serial.println("Master asking slave length of response");
  askForLength();

  if (responseLength == 0) return;
  Serial.print("Response of ");
  Serial.print(responseLength);
  Serial.println(" bytes");

  Serial.println("Master asking slave for response");
  askForData();


  Serial.print(", responseLength: ");
  Serial.print(responseLength);
  Serial.print(" , response from slave: ");
  Serial.println(response);
}

void askForLength() {
  Wire.flush();
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(ASK_FOR_LENGTH);
  Wire.endTransmission();
  Wire.flush();

  int slave_addr = (int)I2C_SLAVE_ADDR;
  Wire.requestFrom(slave_addr, 1);

  while (Wire.available()) {
    responseLength = Wire.read();
  }
  Wire.flush();

  Serial.print("responseLength: ");
  Serial.println(responseLength);
}

void askForData() {
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(ASK_FOR_DATA);
  Wire.endTransmission();
  Wire.flush();

  response = "";

  for (int requestIndex = 0; requestIndex <= (responseLength / 32); requestIndex++) {
    int slave_addr = (int)I2C_SLAVE_ADDR;

    int request_bytes;

    if ((requestIndex + 1) * 32 < responseLength) {

      request_bytes = 32;
    } else {
      request_bytes = responseLength % 32;
    }

    Serial.print("Requesting bytes ");
    Serial.print(requestIndex * 32);
    Serial.print(" to ");
    Serial.println(requestIndex * 32 + request_bytes - 1);

    Wire.requestFrom(slave_addr, request_bytes, 1);
    int c[request_bytes];
    int ii = 0;
    while (Wire.available()) {
      c[ii] = Wire.read();
      String strc = String((char)c[ii]);
      response += strc;
      ii++;
      // Serial.print("Character read: 0x");
      // Serial.println(c[ii],HEX);
    }
    Wire.flush();
    // for(int i=0;i<ii;i++)
    // {
    //   Serial.print("Int ");
    //   Serial.print(i);
    //   Serial.print(" received: ");
    //   Serial.println(c[i]);
    // }
    // Serial.print("Received: ");
    // Serial.println(response);
  }
  // delay(2000);
}


void DeserializeResponse() {
  DeserializationError error = deserializeJson(doc, response);
  //Serial.print("Error in deserializing response: ");
  //Serial.println(error);
  if (error) { return; }

  text = doc["text"];
  id = doc["id"];
  stat = doc["status"];
  value = doc["value"];

  Serial.println(text);
  Serial.println(id);
  Serial.println(stat);
  Serial.println(value);
}