// BLE Service
#include "BLEDevice.h"
BLEScan* pBLEScan;
BLEClient* pClient;

// Variables
static BLEAddress* pServerAddress;
// TPMS BLE SENSORS known addresses
String knownAddresses[] = { "80:ea:ca:12:9b:0a", "81:ea:ca:22:9b:bf", "82:ea:ca:32:90:69", "83:ea:ca:42:90:40" };
// TPMS_FL 80:ea:ca:12:9b:0a
// TPMS_FR 81:ea:ca:22:9b:bf
// TPMS_RL 82:ea:ca:32:90:69
// TPMS_RR 83:ea:ca:42:90:40


String Rss01 = "888";
String Bat01 = "888";
String Tem01 = "88";
String Pre01 = "88";
String Rss02 = "888";
String Bat02 = "888";
String Tem02 = "88";
String Pre02 = "88";
String Rss03 = "888";
String Bat03 = "888";
String Tem03 = "88";
String Pre03 = "88";
String Rss04 = "888";
String Bat04 = "888";
String Tem04 = "88";
String Pre04 = "88";
boolean NewDat = true;
boolean isAlt = true;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
}
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice Device) {
    pServerAddress = new BLEAddress(Device.getAddress());
    bool known = false;
    bool Master = false;
    String ManufData = Device.toString().c_str();
    for (int i = 0; i < (sizeof(knownAddresses) / sizeof(knownAddresses[0])); i++) {
      if (strcmp(pServerAddress->toString().c_str(), knownAddresses[i].c_str()) == 0)
        known = true;
    }
    if (known) {
      String instring = retmanData(ManufData, 0);
      NewDat = true;
      if (instring.substring(4, 6) == "80") {
        Rss01 = Device.getRSSI();
        Bat01 = (returnBatt(instring));
        Tem01 = (returnData(instring, 12) / 100.0);
        Pre01 = (returnData(instring, 8) / 100000.0 * 14.5038);
      } else if (instring.substring(4, 6) == "81") {
        Rss02 = Device.getRSSI();
        Bat02 = (returnBatt(instring));
        Tem02 = (returnData(instring, 12) / 100.0);
        Pre02 = (returnData(instring, 8) / 100000.0 * 14.5038);
      } else if (instring.substring(4, 6) == "82") {
        Rss03 = Device.getRSSI();
        Bat03 = (returnBatt(instring));
        Tem03 = (returnData(instring, 12) / 100.0);
        Pre03 = (returnData(instring, 8) / 100000.0 * 14.5038);
      } else if (instring.substring(4, 6) == "83") {
        Rss04 = Device.getRSSI();
        Bat04 = (returnBatt(instring));
        Tem04 = (returnData(instring, 12) / 100.0);
        Pre04 = (returnData(instring, 8) / 100000.0 * 14.5038);
      }
      if (returnAlarm(instring)) {
        isAlt = true;
      } else {
        isAlt = false;
      }
      Device.getScan()->stop();
      delay(100);
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(100);
  // BLE Init
  BLEDevice::init("");
  pClient = BLEDevice::createClient();
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

void loop() {
  BLEScanResults scanResults = pBLEScan->start(5);
  if (NewDat) {
    Serial.print("\n\nFL: ");
    Serial.print("presion: ");   Serial.print(Pre01);
    Serial.print(" temperatura: ");Serial.print(Tem01);
    Serial.print(" bateria: ");   Serial.print(Bat01);
    Serial.print(" rss: ");     Serial.println(Rss01);

    Serial.print("FR: ");
    Serial.print("presion: ");   Serial.print(Pre02);
    Serial.print(" temperatura: ");Serial.print(Tem02);
    Serial.print(" bateria: ");   Serial.print(Bat02);
    Serial.print(" rss: ");     Serial.println(Rss02);

    Serial.print("RL: ");
    Serial.print("presion: ");   Serial.print(Pre03);
    Serial.print(" temperatura: ");Serial.print(Tem03);
    Serial.print(" bateria: ");   Serial.print(Bat03);
    Serial.print(" rss: ");     Serial.println(Rss03);

    Serial.print("RR: ");
    Serial.print("presion: ");   Serial.print(Pre04);
    Serial.print(" temperatura: ");Serial.print(Tem04);
    Serial.print(" bateria: ");   Serial.print(Bat04);
    Serial.print(" rss: ");     Serial.println(Rss04);

    NewDat = false;
  }
}

// FUNCTIONS
String retmanData(String txt, int shift) {
  // Return only manufacturer data string
  int start = txt.indexOf("data: ") + 6 + shift;
  return txt.substring(start, start + (36 - shift));
}
byte retByte(String Data, int start) {
  // Return a single byte from string
  int sp = (start)*2;
  char* ptr;
  return strtoul(Data.substring(sp, sp + 2).c_str(), &ptr, 16);
}
long returnData(String Data, int start) {
  // Return a long value with little endian conversion
  return retByte(Data, start) | retByte(Data, start + 1) << 8 | retByte(Data, start + 2) << 16 | retByte(Data, start + 3) << 24;
}
int returnBatt(String Data) {
  // Return battery percentage
  return retByte(Data, 16);
}
int returnAlarm(String Data) {
  // Return battery percentage
  return retByte(Data, 17);
}
