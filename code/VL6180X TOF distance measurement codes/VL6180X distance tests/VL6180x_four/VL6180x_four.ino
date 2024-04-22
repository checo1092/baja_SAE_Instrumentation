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
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

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

// Optional define GPIO pins to check to see if complete
//#define GPIO_LOX1 11
//#define GPIO_LOX2 10
//#define GPIO_LOX3 9
//#define GPIO_LOX4 8

#define TIMING_PIN 13

// objects for the VL6180X
Adafruit_VL6180X lox1 = Adafruit_VL6180X();
Adafruit_VL6180X lox2 = Adafruit_VL6180X();
Adafruit_VL6180X lox3 = Adafruit_VL6180X();
Adafruit_VL6180X lox4 = Adafruit_VL6180X();


// Setup mode for doing reads
typedef enum {RUN_MODE_DEFAULT, RUN_MODE_TIMED, RUN_MODE_ASYNC, RUN_MODE_GPIO, RUN_MODE_CONT} runmode_t;

runmode_t run_mode = RUN_MODE_DEFAULT;
uint8_t show_command_list = 1;

//==========================================================================
// Define some globals used in the continuous range mode
// Note: going to start table drive this part, may back up and do the rest later
Adafruit_VL6180X *sensors[] = {&lox1, &lox2, &lox3, &lox4};
const uint8_t COUNT_SENSORS = sizeof(sensors) / sizeof(sensors[0]);
//const int sensor_gpios[COUNT_SENSORS] = {GPIO_LOX1, GPIO_LOX2, GPIO_LOX3, GPIO_LOX4}; // if any are < 0 will poll instead
uint8_t  tempRange;
double          sensor_ranges[COUNT_SENSORS];
uint8_t         sensor_status[COUNT_SENSORS];
// Could do with uint8_t for 8 sensors, but just in case...
const uint16_t  ALL_SENSORS_PENDING = ((1 << COUNT_SENSORS) - 1);
uint16_t        sensors_pending = ALL_SENSORS_PENDING;
uint32_t        sensor_last_cycle_time;


// More variables declaration

long time1 = 0.0;
long time2 = 0.0;
double f = 0.0;

const int buffer_size = 3;
uint8_t buffer[buffer_size][COUNT_SENSORS];
int buffer_index = 0;

double sensor_corr_tab[COUNT_SENSORS][24] = {{12.5, 14, 21.5, 24, 27, 28.5, 33.5, 36, 42.5, 45.5, 52.5, 58, 64, 69, 74.5, 80, 84.5, 88.5, 94.5, 99, 103, 108, 112.5, 116},
                                            {28, 29, 30.5, 32, 36, 38.5, 42, 45, 51, 56, 62, 66, 71.5, 76, 81.5, 86, 91, 97, 101, 106.5, 111, 115, 119, 124},
                                            {1, 2.5, 6, 11, 14, 18.5, 25, 29, 33, 37, 43, 50, 53.5, 59, 64, 68, 72.5, 78, 82, 83, 88.5, 90, 94, 99},
                                            {3, 6, 16.5, 21.5, 24, 26, 29, 33, 36, 41, 43, 48, 53, 58, 61.5, 67, 71.5, 78, 82, 87, 91.5, 95, 99, 103.5}};


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

void readSensor(Adafruit_VL6180X &vl) {

  //Serial.println("Reading lux");
  //float lux = vl.readLux(VL6180X_ALS_GAIN_10);

  //Serial.println("Reading range");
  uint8_t range = vl.readRange();

  // Serial.println("Reading status");
  // uint8_t status = vl.readRangeStatus();

  tempRange = range;
  /*if (status == VL6180X_ERROR_NONE) {
      tempRange = range;//save it for the moment
  }*/

  // Some error occurred, print it out!

  /*if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
    //Serial.print("(System error)");
  }
  else if (status == VL6180X_ERROR_ECEFAIL) {
    //Serial.print("(ECE failure)");
  }
  else if (status == VL6180X_ERROR_NOCONVERGE) {
    //Serial.print("(No convergence)");
  }
  else if (status == VL6180X_ERROR_RANGEIGNORE) {
    //Serial.print("(Ignoring range)");
  }
  else if (status == VL6180X_ERROR_SNR) {
    //Serial.print("Signal/Noise error");
  }
  else if (status == VL6180X_ERROR_RAWUFLOW) {
    //Serial.print("Raw reading underflow");
  }
  else if (status == VL6180X_ERROR_RAWOFLOW) {
    //Serial.print("Raw reading overflow");
  }
  else if (status == VL6180X_ERROR_RANGEUFLOW) {
    //Serial.print("Range reading underflow");
  }
  else if (status == VL6180X_ERROR_RANGEOFLOW) {
    //Serial.print("Range reading overflow");
  }*/
}

void read_sensors() {
  double range = 0;
  //Serial.println("Reading sensor 1");
  readSensor(lox1);
  //range = tempRange*0.988;
  buffer[buffer_index][0] = tempRange;//save it now
  sensor_ranges[0] = sensor_adjust(0,range_average(0));//round(range_average(0));

  readSensor(lox2);
  //range = tempRange*1.04;
  buffer[buffer_index][1] = tempRange; //save it now 
  sensor_ranges[1] = sensor_adjust(1,range_average(1));//round(range_average(1));

  readSensor(lox3);
  //range = pow(tempRange*1.0,2)*0.002 + tempRange; // Adjust range
  buffer[buffer_index][2] = tempRange; //save it now 
  sensor_ranges[2] = sensor_adjust(2,range_average(2));//round(range_average(2));//

  readSensor(lox4);
  //range = tempRange*1.169; // Adjust range
  buffer[buffer_index][3] = tempRange; //save it now 
  sensor_ranges[3] = sensor_adjust(3,range_average(3));//round(range_average(3));//

  buffer_index++;
  if (buffer_index >= buffer_size)
  {
    buffer_index = 0;
  }

  Serial.println();
}

double range_average(int num_sensor)
{
  long sum = 0;
  for (int i = 0; i < buffer_size; i++)
  {
    sum += (long)buffer[i][num_sensor];
  }
  return (double)(sum*1.0/buffer_size);
}

double sensor_adjust(int num_sensor, double range)
{
  int ind_min = linearSearch(num_sensor,range);
  double range_diff = sensor_corr_tab[num_sensor][ind_min] - range;
  int ind_1,ind_2;
  double val1,val2;
  double interp;

  ind_1 = ind_min;
  ind_2 = ind_min;

  // Serial.print("ind_min: ");
  // Serial.println(ind_min);
  // Serial.print("sensor_corr_tab[num_sensor][ind_min]: ");
  // Serial.println(+sensor_corr_tab[num_sensor][ind_min]);
  // Serial.print("range: ");
  // Serial.println(range);
  // Serial.print("range_diff: ");
  // Serial.println(range_diff);

  if(range_diff == 0)
  {
    // Serial.println("range_diff == 0");
    return ind_min*5.0;
  }
  else if(range_diff > 0)
  {
    // Serial.println("range_diff > 0");
    if(ind_min < 0)
    {
      ind_1 -= 1;
      // ind_2 = ind_min;
      val1 = sensor_corr_tab[num_sensor][ind_1];
      val2 = sensor_corr_tab[num_sensor][ind_2];
      interp = (ind_1*1.0 + (range-val1)*1.0/(val2-val1))*5.0;
      return interp;
    }
    else
    {
      return ind_min*5.0;
    }

  }
  else if(range_diff < 0)
  {
    // Serial.println("range_diff < 0");
    if(ind_min < 24-1)
    {
      // ind_1 = ind_min;
      ind_2 += 1;
      val1 = sensor_corr_tab[num_sensor][ind_1];
      val2 = sensor_corr_tab[num_sensor][ind_2];
      interp = (ind_1*1.0 + (range-val1)*1.0/(val2-val1))*5.0;
      // Serial.print("val1: ");
      // Serial.println(val1);
      // Serial.print("val2: ");
      // Serial.println(val2);
      // Serial.print("interp: ");
      // Serial.println(interp);  
      return interp;
    }
    else
    {
      return ind_min*5.0;
    }
  }
}

int linearSearch(int num_sensor, double range)
{
  double min = abs(sensor_corr_tab[num_sensor][0] - range);
  int ind_min = 0;
  double range_diff;
  for(int i = 0; i < 24 - 1;i++)
  {
    range_diff = abs(sensor_corr_tab[num_sensor][i] - range);
    if(range_diff < min)
    {
      ind_min = i;
      min = range_diff;
    }
  }
  return ind_min;
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
    String str_ranges = String(sensor_ranges[i],1);
    Serial.print(str_ranges);

    // Serial.print("mm");
    if(i==COUNT_SENSORS) Serial.println();
    else Serial.print(",");
  }
}

String json;
StaticJsonDocument<300> doc;

void SerializeObject(long time1, long time2)
{
    doc["t1"] = time1;
    doc["t2"] = time2;
    doc["s1"] = String(sensor_ranges[0],1);
    doc["s2"] = String(sensor_ranges[1],1);
    doc["s3"] = String(sensor_ranges[2],1);
    doc["s4"] = String(sensor_ranges[3],1);
 
    serializeJson(doc, json);
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

#ifdef GPIO_LOX1
  // If we defined GPIO pins, enable them as PULL UP
  pinMode(GPIO_LOX1, INPUT_PULLUP);
  pinMode(GPIO_LOX2, INPUT_PULLUP);
  pinMode(GPIO_LOX3, INPUT_PULLUP);
  pinMode(GPIO_LOX4, INPUT_PULLUP);

#endif

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

String str_ranges;

//===============================================================
// Loop
//===============================================================
void loop() {
  //Serial.print("Reading ");
  //Serial.print(COUNT_SENSORS);
  //Serial.println(" sensors");
  time1 = micros();
  read_sensors();
  time2 = micros();
  //calculate_frec(time1,time2);


  ranges_SerialPrint();

  // SerializeObject(time1,time2);
  // Serial.print(json);

  // delay(1000);
}