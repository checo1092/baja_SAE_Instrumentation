#include "Wire.h"
#include "Arduino.h"
#include "LoRa_E220.h"

const byte I2C_SLAVE_ADDR = 0x20;
const int num_VL6180 = 4;

const int dataset_size = 8;
const int dataset_LORA_size = 3;
unsigned char dist_data[num_VL6180*dataset_size];

int ind_dataset_LORA = 0;
int ind_dataset_LORA_ant = 0;
int ind_data = 0;

String data = "";
String message = "";
String response = "";

long time1 = 1;
long time2 = 2;

const byte tx_pin = 16;
const byte rx_pin = 17;
const byte aux_pin = 18;
const byte M0_pin = 25;
const byte M1_pin = 23;

LoRa_E220 e220ttl(&Serial2, aux_pin, M0_pin, M1_pin);  //  RX AUX M0 M1
// #define DESTINATION_ADDL 3

void setup()
{
  Serial.begin(115200, SERIAL_8N1); // Write to OpenLog
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveEvent);

  // pinMode(M0_pin, OUTPUT);    // set M0 low
  // digitalWrite(M0_pin, LOW);
  // pinMode(M1_pin, OUTPUT);    // set M1 low
  // digitalWrite(M1_pin, LOW);
  // Serial2.begin(9600, SERIAL_8N1); // Write to LORA

  while(e220ttl.begin() == 0)
  {
    Serial.println("LoRa e220 not set up");
    delay(1000);
  }

  Serial.println("LoRa e220 is up");
  loadDefaultConfig(e220ttl);
  printLORAConfig(e220ttl);

  Serial.println("Init complete");


  // delay(500);
  delay(2000);
}


void receiveEvent(int bytes)
{
  data = "";
  ind_data = 0;
  int incomming_data = 0;
  while (Wire.available())
  {
    //byte* pointer = (byte*)&data;
    incomming_data = Wire.read();
    if(ind_data < num_VL6180*dataset_size)
    {
      dist_data[ind_data] = (unsigned char)incomming_data;
      dist_data[ind_data] += 2; // Avoid NUL CHAR = 0, SOH CHAR = 1
      data += String((char)incomming_data);
    }
    ind_data++;
  }

  for(int i = 0; i < num_VL6180*dataset_size; i++)
  {
    Serial.print(dist_data[i]);
    Serial.print(", ");
  }
  Serial.println();

  // Serial.print("Received ");
  // Serial.print(ind_data);
  // Serial.println(" bytes");
}

double f = 0;

void ranges_SerialPrint()
{
  for(int i = 0; i < num_VL6180*dataset_size; i++)
  {
    //Serial.print("Sensor ");
    //Serial.print(i);
    //Serial.print(" :");
    Serial.print(dist_data[i]);

    // Serial.print("mm");
    // if(i==num_VL6180*dataset_size) Serial.println();
    // else Serial.print(",");
    if((i+1)%4 == 0 && i != 0 && i < num_VL6180*dataset_size - 1)
    {
      Serial.print(";");
    }
    else if(i < num_VL6180*dataset_size - 1)
    {
      Serial.print(",");
    }
    if(i == num_VL6180*dataset_size - 1)
    {
      Serial.println();
    }
  }
  // Serial.print(sizeof(dist_data));
  Serial.println(";");
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


void loop() {
  // Serial.println("In loop");

  if(ind_data == num_VL6180*dataset_size)
  {
    // Serial.print("Building LORA message, ");
    // Serial.print("ind_data: ");
    // Serial.print(ind_data);
    // Serial.print(", ind_dataset_LORA: ");
    // Serial.println(ind_dataset_LORA);

    // Serial.print("Aquiring 1/");
    // Serial.print(dataset_LORA_size);
    // Serial.println(" of message");

    // Serial.print("size of message: ");
    // Serial.println(message.length());

    // Serial.print("size of data: ");
    // Serial.println(data.length());

    message += data;
    ind_dataset_LORA++;
    ind_data = 0;
  }
  if (ind_dataset_LORA >= dataset_LORA_size)
  {
    // Serial.print("dataset_LORA_size: ");
    // Serial.println(ind_dataset_LORA);

    // message += message.substring(0,message.length()/3);
    Serial.print("Send ");
    Serial.print(message.length());
    Serial.println(" to LORA receiver");

    // Serial.print("ind_data: ");
    // Serial.println(ind_data);
    time2 = micros();

    Serial.flush();
    Serial2.flush();
    sendLORAmessage(message);
    // ranges_SerialPrint();
    // Serial.print("message: ");
    // Serial.println(message);

    for(int i = 0;i <message.length();i++)
    {
      Serial.print((int)message.charAt(i));
      if((i+1)%4 == 0 && i != 0 && i < message.length() - 1)
      {
        Serial.print(";");
      }
      else if(i < message.length() - 1)
      {
        Serial.print(",");
      }
      if(i == message.length() - 1)
      {
        Serial.println();
      }
      
    }
    // Serial.println();
    // Serial.println();

    // calculate_frec(time1, time2);
    // data = "";
    ind_data = 0;
    ind_dataset_LORA = 0;
    message = "";

    // Serial.print("loop() running on core ");
    // Serial.println(xPortGetCoreID());
    // Wire.begin(I2C_SLAVE_ADDR);
    time1 = micros();
  }
  
  // if (e220ttl.available()>1) 
  // {
  //   response = readLORAIncomingData(0);
  //   if(response.compareTo("") != 0)
  //   {
  //     Serial.println(response);
  //   }
  // }
  // if (Serial.available()) 
  // {
  //   sendLORAIncomingSerialPortData();
  // }
}


String receiveLORAmessage(LoRa_E220 e220ttl)
{
  String response;
  // Serial.println("Waiting for initial response");
  if (e220ttl.available()>1) 
  {
    response = readLORAIncomingData(0);
    if(response.compareTo("") != 0)
    {
      // Serial.println(response);
      return response;
    }
    else
    {
      return "";
    }
  }
  else
  {
    return "";
  }
}

ResponseStatus sendLORAmessage(String message)
{
  ResponseStatus rs;
  // Serial.println(message);
  rs = e220ttl.sendMessage(message);//0, dest_addr, channel, message);
  // Serial.println(rs.getResponseDescription());
  if(rs.getResponseDescription().compareTo("Success") != 0)
  {
    Serial.print("An error ocurred: ");
    Serial.println(rs.getResponseDescription());
  }
  return rs;
}

void sendLORAIncomingSerialPortData()
{
  String input = Serial.readString();

  ResponseStatus rs = sendLORAmessage(input);

  Serial.print("Sent: ");
  Serial.print(input);
}

String readLORAIncomingData(int verbose)
{

  if(verbose == 1)
  {
    Serial.println("Recieved something...");
  }
  // read the String message
  ResponseContainer rc = e220ttl.receiveMessage();

  // Is something goes wrong print error
  if (rc.status.code!=1)
  {
    if(verbose == 1)
    {
      Serial.print("Got an error: ");
      Serial.println(rc.status.getResponseDescription());
    }
    return "";
  }
  else
  {
    // Print the data received
    if(verbose == 1)
    {
      Serial.print("Received data correctly: ");
      Serial.println(rc.status.getResponseDescription());
      Serial.println(rc.data);
    }
    return rc.data;
  }
}

void printLORAConfig(LoRa_E220 e220ttl)
{
  ResponseStructContainer c;
  c = e220ttl.getConfiguration();
  // It's important get configuration pointer before all other operation
  Configuration configuration = *(Configuration*) c.data;

  String c_status = c.status.getResponseDescription();
  while(!(c_status.compareTo("Success") == 0))
  {
    Serial.println(c.status.code);
    Serial.println(c_status);
    delay(1000);
    c_status = c.status.code;
  }
  Serial.println(c.status.code);
  Serial.println(c_status);

  printLORAParameters(configuration);
  c.close();
}

void printLORAParameters(struct Configuration configuration) {
    Serial.println("----------------------------------------");
 
    Serial.print(F("HEAD : "));  Serial.print(configuration.COMMAND, HEX);Serial.print(" ");Serial.print(configuration.STARTING_ADDRESS, HEX);Serial.print(" ");Serial.println(configuration.LENGHT, HEX);
    Serial.println(F(" "));
    Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, HEX);
    Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, HEX);
    Serial.println(F(" "));
    Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
    Serial.println(F(" "));
    Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte     : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRateDescription());
    Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRateDescription());
    Serial.println(F(" "));
    Serial.print(F("OptionSubPacketSett: "));  Serial.print(configuration.OPTION.subPacketSetting, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getSubPacketSetting());
    Serial.print(F("OptionTranPower    : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());
    Serial.print(F("OptionRSSIAmbientNo: "));  Serial.print(configuration.OPTION.RSSIAmbientNoise, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getRSSIAmbientNoiseEnable());
    Serial.println(F(" "));
    Serial.print(F("TransModeWORPeriod : "));  Serial.print(configuration.TRANSMISSION_MODE.WORPeriod, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
    Serial.print(F("TransModeEnableLBT : "));  Serial.print(configuration.TRANSMISSION_MODE.enableLBT, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
    Serial.print(F("TransModeEnableRSSI: "));  Serial.print(configuration.TRANSMISSION_MODE.enableRSSI, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
    Serial.print(F("TransModeFixedTrans: "));  Serial.print(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());
 
    Serial.println("----------------------------------------");
}

void senderConfig(LoRa_E220 e220ttl){
  ResponseStructContainer c;
  c = e220ttl.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  configuration.ADDL = 0x02;
  configuration.ADDH = 0x00;
  
  configuration.CHAN = 23;
  
  configuration.SPED.uartBaudRate = UART_BPS_9600;
  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
  configuration.SPED.uartParity = MODE_00_8N1;
  
  configuration.OPTION.subPacketSetting = SPS_200_00;
  configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_ENABLED;
  configuration.OPTION.transmissionPower = POWER_22;
  
  configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
  configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
  configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
  e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  c.close();
}

void receiverConfig(LoRa_E220 e220ttl){
  ResponseStructContainer c;
  c = e220ttl.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  configuration.ADDL = 0x03;
  configuration.ADDH = 0x00;
  
  configuration.CHAN = 23;
  
  configuration.SPED.uartBaudRate = UART_BPS_9600;
  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
  configuration.SPED.uartParity = MODE_00_8N1;
  
  configuration.OPTION.subPacketSetting = SPS_200_00;
  configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
  configuration.OPTION.transmissionPower = POWER_22;
  
  configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
  configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
  configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
  e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  c.close();
}

void loadDefaultConfig(LoRa_E220 e220ttl){
  //  If you have ever change configuration you must restore It
    ResponseStructContainer c;
    c = e220ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;
    Serial.println(c.status.getResponseDescription());
    configuration.ADDL = 0x03;  // First part of address
    configuration.ADDH = 0x00; // Second part
 
    configuration.CHAN = 40; // Communication channel
 
    configuration.SPED.uartBaudRate = UART_BPS_9600; // Serial baud rate
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24; // Air baud rate
    configuration.SPED.uartParity = MODE_00_8N1; // Parity bit
 
    configuration.OPTION.subPacketSetting = SPS_200_00; // Packet size
    configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED; // Need to send special command
    configuration.OPTION.transmissionPower = POWER_22; // Device power
 
    configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED; // Enable RSSI info
    configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION; // Enable repeater mode
    configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED; // Check interference
    configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011; // WOR timing
    e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    c.close();
}