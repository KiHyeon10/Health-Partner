#include <ArduinoBLE.h>

//87654321-4321-4321-4321-abcdefabcdef
BLEService myService("12345678-1234-1234-1234-123456789abc");
BLEIntCharacteristic myCharacteristic("87654321-4321-4321-4321-abcdefabcdef", BLERead | BLENotify);
BLEIntCharacteristic ReadApp("12345678-1234-1234-1234-123456789abc", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1)
      ;
  }

  BLE.setLocalName("ArduinoBLE33");
  BLE.setAdvertisedService(myService);

  myService.addCharacteristic(myCharacteristic);
  myService.addCharacteristic(ReadApp);
  BLE.addService(myService);

  myCharacteristic.writeValue(0);
  ReadApp.writeValue(0);

  BLE.advertise();
  Serial.println("BLE device active, waiting for connections...");
}

float a = 1000;

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      myCharacteristic.writeValue(123);
      //ReadApp.writeValue(6969);
      //ReadApp.readValue();
      
      if(ReadApp.written()){
        unsigned int val = ReadApp.value();
        Serial.println(val);
      }
    }
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
