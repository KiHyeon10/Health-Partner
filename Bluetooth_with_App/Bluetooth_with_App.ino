#include <ArduinoBLE.h>

BLEService DeviceInformation("180A");
BLECharCharacteristic Weight("2A98", BLEWrite | BLERead | BLENotify);  
BLECharCharacteristic WeightMeasurement("2A9D", BLEWrite | BLERead | BLENotify); 

void setup() {

  Serial.begin(9600);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("BLE chat machine");
  BLE.setAdvertisedService(DeviceInformation); 
  DeviceInformation.addCharacteristic(Weight); 
  DeviceInformation.addCharacteristic(WeightMeasurement); 
  BLE.addService(DeviceInformation);

  // set the initial value for the characeristic:
  Weight.writeValue(NULL);
  WeightMeasurement.writeValue(NULL);

  // start advertising
  BLE.advertise();
  Serial.println("BLE LED Peripheral");
}

void loop() {

  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {

    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if(Serial.available()){

        char val1 = Serial.read();
        Weight.writeValue(val1);
        Serial.println(val1);
      }

      char val2 = WeightMeasurement.value();
      if (val2 != NULL){

        Serial.println(val2);
        WeightMeasurement.writeValue(NULL);
      }
    }
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}