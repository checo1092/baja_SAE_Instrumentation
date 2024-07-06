 /*
 *  Arduino code 
Using two or more VL6180X 20cm Time-of-Flight proximity sensor with Arduino

View code for using single VL6180X sensors: https://robojax.com/learn/arduino/?vid=robojax_VL6180X_laser

 * Original code and library by https://github.com/adafruit/Adafruit_VL6180X
 * 
 * Written/updated by Ahmad Shamshiri for Robojax Robojax.com
 * on Mar 12, 2021  in Ajax, Ontario, Canada
 Watch the video instruction for this sketch: https://youtu.be/_H9D0czQpSI
 


If you found this tutorial helpful, please support me so I can continue creating 
content like this. You can support me on Patreon http://robojax.com/L/?id=63
or make donation using PayPal http://robojax.com/L/?id=64

* 
 * Code is available at http://robojax.com/learn/arduino

 * This code is "AS IS" without warranty or liability. Free to be used as long as you keep this note intact.* 
 * This code has been download from Robojax.com
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. 

*/

#include <Adafruit_VL6180X.h>
#include "Wire.h"

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32
#define LOX4_ADDRESS 0x33

// set the pins to shutdown
#define SHT_LOX1 2
#define SHT_LOX2 0
#define SHT_LOX3 16
#define SHT_LOX4 17

#define TIMING_PIN 13

// objects for the VL6180X
Adafruit_VL6180X lox1 = Adafruit_VL6180X();
Adafruit_VL6180X lox2 = Adafruit_VL6180X();
Adafruit_VL6180X lox3 = Adafruit_VL6180X();
Adafruit_VL6180X lox4 = Adafruit_VL6180X();

//==========================================================================
// Define some globals used in the continuous range mode
Adafruit_VL6180X *sensors[] = {&lox1, &lox2, &lox3, &lox4};
const uint8_t COUNT_SENSORS = sizeof(sensors) / sizeof(sensors[0]);
unsigned char sensor_ranges[COUNT_SENSORS];

long time1 = 0.0;
long time2 = 0.0;
double f = 0.0;

const int buffer_size = 3;
unsigned char buffer[buffer_size][COUNT_SENSORS];
int buffer_index = 0;

const byte I2C_SLAVE_ADDR = 0x20;
int requestIndex = 0;

const int dataset_size = 8;
int ind_data_set = 0;
unsigned char sensor_ranges_dataset[COUNT_SENSORS*dataset_size];

/*
    Reset all sensors by setting all of their XSHUT pins low for delay(10), then set all XSHUT high to bring out of reset
    Keep sensor #1 awake by keeping XSHUT pin high
    Put all other sensors into shutdown by pulling XSHUT pins low
    Initialize sensor #1 with lox.begin(new_i2c_address) Pick any number but 0x29 and it must be under 0x7F. Going with 0x30 to 0x3F is probably OK.
    Keep sensor #1 awake, and now bring sensor #2 out of reset by setting its XSHUT pin high.
    Initialize sensor #2 with lox.begin(new_i2c_address) Pick any number but 0x29 and whatever you set the first sensor to
*/
void setID() {
  Serial.println("Setting ID init");
  // all reset
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  digitalWrite(SHT_LOX4, LOW);

  delay(10);

  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  digitalWrite(SHT_LOX3, HIGH);
  digitalWrite(SHT_LOX4, HIGH);

  delay(10);

  // all deactivated
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  digitalWrite(SHT_LOX4, LOW);

  // init all sensors
  init_VL6180X(lox1,SHT_LOX1,LOX1_ADDRESS);
  init_VL6180X(lox2,SHT_LOX2,LOX2_ADDRESS);
  init_VL6180X(lox3,SHT_LOX3,LOX3_ADDRESS);
  init_VL6180X(lox4,SHT_LOX4,LOX4_ADDRESS);

  Serial.println(F("All VL6180X sensors are ready"));
  delay(1000);

 
}

void init_VL6180X(Adafruit_VL6180X &vl, int sht_pin, int address)
{
  digitalWrite(sht_pin, HIGH);
  // initing vl
  while (!vl.begin()) {

    Serial.print(F("Failed to boot VL6180X at address 0x"));
    Serial.println(address,HEX);

    delay(100);
  }
  vl.setAddress(address);

  //vl.write8(0x0011, 0x10); // SYSTEM__MODE_GPIO1 // NO TOCAR
  //vl.write8(0x10a,48); // READOUT__AVERAGING_SAMPLE_PERIOD // IMPORTANTE
  // vl.write8(0x0024,0x00); // SYSRANGE__PART_TO_PART_RANGE_OFFSET // IMPORTANTE
  // vl.write8(0x003f, 0x46); // SYSALS__ANALOGUE_GAIN // NO RELEVANTE
  // vl.write8(0x0031, 0xFF); // SYSRANGE__VHV_REPEAT_RATE // Recalibracion // NO TOCAR
  // vl.write8(0x002e, 0x01); // SYSRANGE__VHV_RECALIBRATE // Recalibracion // NO TOCAR
  // vl.write8(0x0040, 0x63); // SYSALS__INTEGRATION_PERIOD // IMPORTANTE
  // vl.write8(0x001B,0x09); // SYSRANGE__INTERMEASUREMENT_PERIOD // NO RELEVANTE, solo para modo continuo
  // vl.write8(0x003e, 0x31); // SYSALS__INTERMEASUREMENT_PERIOD // NO RELEVANTE
  // vl.write8(0x0014, 0x24); // SYSTEM__INTERRUPT_CONFIG_GPIO // NO RELEVANTE

  Serial.print(F("Sensor VL6180X at address 0x"));
  Serial.print(address,HEX);
  Serial.println(F(" is set up!"));

  delay(10);
  //digitalWrite(sht_pin, LOW);
  
}

void read_sensors() {

  // Serial.println("Reading sensor 1");
  buffer[buffer_index][0] = lox1.readRange();
  sensor_ranges[0] = range_average(0);

  // Serial.println("Reading sensor 2");
  buffer[buffer_index][1] = lox2.readRange();
  sensor_ranges[1] = range_average(1);

  // Serial.println("Reading sensor 3");
  buffer[buffer_index][2] = lox3.readRange();
  sensor_ranges[2] = range_average(2);

  // Serial.println("Reading sensor 4");
  buffer[buffer_index][3] = lox4.readRange();
  sensor_ranges[3] = range_average(3);

  buffer_index++;
  if (buffer_index >= buffer_size)
  {
    buffer_index = 0;
  }

}

unsigned char range_average(int num_sensor)
{
  int sum = 0;
  for (int i = 0; i < buffer_size; i++)
  {
    sum += (int)buffer[i][num_sensor];
  }
  // Serial.print(sum);
  // Serial.print(",");
  // Serial.println(round((sum*1.0/buffer_size)));
  return (unsigned char)round((sum*1.0/buffer_size));
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

void ranges_SerialPrint()
{
  for(int i = 0; i < COUNT_SENSORS; i++)
  {
    //Serial.print("Sensor ");
    //Serial.print(i);
    //Serial.print(" :");
    Serial.print(sensor_ranges[i]);

    // Serial.print("mm");
    if(i==COUNT_SENSORS) Serial.println();
    else Serial.print(",");
  }
  // Serial.print(sizeof(sensor_ranges));
  // Serial.println(";");
}

//===============================================================
// Setup
//===============================================================
void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  pinMode(SHT_LOX3, OUTPUT);
  pinMode(SHT_LOX4, OUTPUT);

  // Enable timing pin so easy to see when pass starts and ends
  pinMode(TIMING_PIN, OUTPUT);
  
  Serial.println("Shutdown pins inited...");

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  digitalWrite(SHT_LOX4, LOW);

  digitalWrite(TIMING_PIN, LOW);
  Serial.println("All in reset mode...(pins are low)");


  Serial.println("Starting...");
  Serial.println();
  setID();
  Wire.begin();

}

void send_i2c_Data()
{
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  int data_size = sizeof(sensor_ranges_dataset);
  int sent_size = 32;

  for(int i = 0;i < 1;i++)
  {
    if((data_size - i*32) < 32)
    {
      sent_size = (data_size - i*32);
    }

    // Serial.print("Sent ");
    // Serial.print(sent_size);
    // Serial.println(" bytes");

    Wire.write(&sensor_ranges_dataset[i], sent_size);
  }
  Wire.endTransmission();
}

//===============================================================
// Loop
//===============================================================
void loop() {
  // Serial.print("Reading ");
  // Serial.print(COUNT_SENSORS);
  // Serial.println(" sensors");
  read_sensors();
  // delay(8);


  // Serial.print("ind_data_set: ");
  // Serial.println(ind_data_set);
  if(ind_data_set < dataset_size)
  {
    for(int i = 0; i < COUNT_SENSORS; i++)
    {
      sensor_ranges_dataset[ind_data_set*COUNT_SENSORS + i] = sensor_ranges[i];
    }
    ind_data_set++;
  }
  if(ind_data_set == dataset_size)
  {
    send_i2c_Data();
    for(int i = 0; i < ind_data_set*COUNT_SENSORS;i++)
    {
      Serial.print(sensor_ranges_dataset[i]);
      if((i+1)%4 == 0 && i != 0 && i < ind_data_set*COUNT_SENSORS - 1)
      {
        Serial.print(";");
      }
      else if(i < ind_data_set*COUNT_SENSORS - 1)
      {
        Serial.print(",");
      }
      if(i == ind_data_set*COUNT_SENSORS - 1)
      {
        Serial.println();
      }
    }
    Serial.println();
    ind_data_set = 0;
    // delay(250);
    time2 = micros();
    calculate_frec(time1,time2);
    time1 = micros();
    Serial.println();
    // delay(250);
  }

  // ranges_SerialPrint();
}