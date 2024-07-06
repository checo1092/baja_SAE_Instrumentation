#include "Arduino.h"
#include "LoRa_E220.h"

String data;
String response = "";
String message = "";

int ind_data = 0;

byte tx_pin = 16;
byte rx_pin = 17;
byte aux_pin = 18;
byte M0_pin = 25;
byte M1_pin = 23;

LoRa_E220 e220ttl(&Serial2, 18, 25, 23);  //  RX AUX M0 M1
#define DESTINATION_ADDL 3

void setup() {
  Serial.begin(9600, SERIAL_8N1);           //OpenLog
  // Serial1.begin(9600, SERIAL_8N1, 19, 18);  //19-TX 18-RX for GPS for slave esp32

  delay(500);
  // Startup all pins and UART
  
  while(e220ttl.begin() == 0)
  {
    Serial.println("LoRa e220 not set up");
    delay(1000);
  }

  // Serial.println("LoRa e220 is up");

  loadDefaultConfig(e220ttl);
  printLORAConfig(e220ttl);
  // senderConfig(e220ttl);
  // printLORAConfig(e220ttl);

  message = "Hello world? Sender is ready";
  ResponseStatus rs;

  while(response.compareTo("") == 0)
  {
    do
    {
      Serial.println(message);
      rs = sendLORAmessage(message);
      delay(1000);
    }while(rs.getResponseDescription().compareTo("Success") != 0);

    response = receiveLORAmessage(e220ttl);
  }
  Serial.println(response);

  Serial.println("Start sending data");
  delay(1000);
  
}

void loop() {
  // Send a number
  if(ind_data < 4)
  {
    data += String(millis()) + ",";// + String(ind_data)+";";
    ind_data++;
  }
  else
  {
      Serial.println(data);
      sendLORAmessage(data);
    ind_data = 0;
  }
  Serial.print("ind_data");
  Serial.println(ind_data);

  if (e220ttl.available()>1) 
  {
    response = readLORAIncomingData(0);
    if(response.compareTo("") != 0)
    {
      Serial.println(response);
    }
  }
  if (Serial.available()) 
  {
    sendLORAIncomingSerialPortData();
  }

  delay(100);
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
  
  configuration.SPED.uartBaudRate = UART_BPS_19200;
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
 
    configuration.CHAN = 23; // Communication channel
 
    configuration.SPED.uartBaudRate = UART_BPS_19200; // Serial baud rate
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
