#include "BLEDevice.h"
#include "EasyBuzzer.h"

unsigned int frequency = 1000;  
unsigned int onDuration = 50;
unsigned int offDuration = 100;
unsigned int beeps = 2;
unsigned int pauseDuration = 500;
unsigned int cycles = 10;

int LED = 2; // on-board LED at pin 2
int BUTTON = 0;
static BLEAddress *pServerAddress;
BLEScan* pBLEScan;
BLEClient*  pClient;
bool deviceFound = false;
bool LEDoff = false;
bool BotonOff = false;
String knownAddresses[] = { "e0:a1:07:b7:0b:95"}; // change the MAC
unsigned long entry;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice Device){
      // show the MAC of other BLE devices
      //Serial.print("BLE Advertised Device found: ");
      //Serial.println(Device.toString().c_str());
      pServerAddress = new BLEAddress(Device.getAddress()); 
      bool known = false;
      bool Master = false;
      for (int i = 0; i < (sizeof(knownAddresses) / sizeof(knownAddresses[0])); i++) {
        if (strcmp(pServerAddress->toString().c_str(), knownAddresses[i].c_str()) == 0) 
          known = true;
      }
      if (known) {
        Serial.print("Our device found!");
        Serial.print("Device distance:");
        Serial.println(Device.getRSSI());
        // adjust the value. -85 is medium distance
        // -60 is closer than -85
        if (Device.getRSSI() > -85) {
          deviceFound = true;
        }
        else {
          deviceFound = false;
        }
        Device.getScan()->stop();
        delay(100);
      }
    }
};
void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  BLEDevice::init("");
  pClient  = BLEDevice::createClient();
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  Serial.println("Done");
}
void Bluetooth() {
  Serial.println();
  Serial.println("BLE Scan restarted.....");
  deviceFound = false;
  BLEScanResults scanResults = pBLEScan->start(5);
  if (deviceFound) {
    Serial.println("LED is ON now");
    LEDoff = true;
    digitalWrite(LED,HIGH);
    EasyBuzzer.beep(frequency, onDuration, offDuration, beeps, pauseDuration, cycles);
    BUTTON = 0;
    delay(1000);
  }
  else{
    EasyBuzzer.stopBeep();
    digitalWrite(LED,LOW);
    delay(1000);
  }
}
void loop() { 
  Bluetooth();
  EasyBuzzer.update();
}
