## Code


*** 
## Files
1. /GPS measurement codes    ESP32WROOM with GY-GPSU3-NEO module for GPS measurements. 
2. /LORA transmition codes   ESP32WROOM with E220-900T30D module for LORA information transmitions. 
3. /I2C transmition codes    ESP32WROOM I2C communication. 
4. /tpmsReading              
5. /transmission_Test

***

## Instalation of the ESP32 board and libraries into the arduino IDE.
1. Install the boad adding https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json to the additional board manager URLs field.	
2. In the board manager search ESP32 and install ESP32 by Espressif Systems.
3. Install the "LoRa" library by sandeep mistry. 
4. Install the "Adafruit SSD1306" library by Adafruit. (TTGO Dispaly version)
5. Install the "Adafruit GFX Library" by Adafruit. (TTGO Display version)
6. Install the "Adafruit_VL6180X library" by Adafruit. (VL6180X TOF distance sensor)
7. Install the "ArduinoJson" library by Benoit Blanchon. 

## Tested codes

For distance measurements using the VL6180X TOF distance sensor, use "VL6180x_four" Arduino code. 

## On development

 - Create JSON data structure and send via I2C communication with other ESP32WROOM
 - Test LoRa codes for sensor measurements remote monitoring. 

## Notes on VL6180X TOF distance measurements

It is necesary to modify Adafruit_VL6180X.cpp file. 

Add these lines at the end of method Adafruit_VL6180X::loadSettings(void): 

  write8(0x0024, 0x14); // SYSRANGE__PART_TO_PART_RANGE_OFFSET
  write8(0x010a, 0x18); // READOUT__AVERAGING_SAMPLE_PERIOD
  write8(0x001c, 0x05); //SYSRANGE__MAX_CONVERGENCE_TIME

These lines access registers of VL6180X internal microcontroller. They modify measurement parameters for used mode "single range measurement", see datasheet for default values. 


