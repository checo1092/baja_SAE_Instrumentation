/* This example demonstrates how to use interleaved mode to
take continuous range and ambient light measurements. The
datasheet recommends using interleaved mode instead of
running "range and ALS continuous modes simultaneously (i.e.
asynchronously)".

In order to attain a faster update rate (10 Hz), the max
convergence time for ranging and integration time for
ambient light measurement are reduced from the normally
recommended defaults. See the section "Continuous mode
limits" and the table "Interleaved mode limits (10 Hz
operation)" in the VL6180X datasheet for more details.

Raw ambient light readings can be converted to units of lux
using the equation in datasheet section "ALS count
to lux conversion".

Example: A VL6180X gives an ambient light reading of 613
with the default gain of 1 and an integration period of
50 ms as configured in this sketch (reduced from 100 ms as
set by configureDefault()). With the factory calibrated
resolution of 0.32 lux/count, the light level is therefore
(0.32 * 613 * 100) / (1 * 50) or 392 lux.

The range readings are in units of mm. */

#include <Wire.h>
#include <VL6180X.h>

#define SHTPIN 19

VL6180X sensor;

void setup()
{
  pinMode(SHTPIN, OUTPUT);
  digitalWrite(SHTPIN,LOW);

  delay(2000);

  digitalWrite(SHTPIN,HIGH);

  delay(2000);

  Serial.begin(9600);
  // Serial.println("Serial monitor ON ");
  Wire.begin();

  // Serial.println("Config sensor init");
  sensor.init();
  sensor.configureDefault();
  // Serial.println("Config sensor fin");

  // Reduce range max convergence time and ALS integration
  // time to 30 ms and 50 ms, respectively, to allow 10 Hz
  // operation (as suggested by table "Interleaved mode
  // limits (10 Hz operation)" in the datasheet).
  sensor.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 5);
  sensor.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 1);
  sensor.writeReg(VL6180X::READOUT__AVERAGING_SAMPLE_PERIOD, 6);
  //sensor.writeReg(VL6180X::SYSALS__INTERMEASUREMENT_PERIOD,254);

  sensor.setTimeout(500);

   // stop continuous mode if already active
  sensor.stopContinuous();
  // in case stopContinuous() triggered a single-shot
  // measurement, wait for it to complete
  delay(300);
  // start interleaved continuous mode with period of 100 ms
  sensor.startInterleavedContinuous(15);

  Serial.println("All Config sensor fin");
  //sensor.setScaling(1);
}

double time1 = 0.0;
double time2 = 0.0;
double f = 0.0;
uint8_t md = 0;

void loop()
{
  //Serial.print("Ambient: ");
  //Serial.print(sensor.readAmbientContinuous());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  //Serial.print(",");
  //Serial.print("\tRange: ");

  time1 = micros();
  md = sensor.readRangeContinuous();
  time2 = micros();
  f = 1000000/(time2-time1);

  Serial.print(md,DEC);

  
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  //Serial.print(", f: ");
  //Serial.print(f);

  Serial.println();
}
