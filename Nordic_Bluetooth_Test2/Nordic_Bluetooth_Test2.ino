#include <ArduinoBLE.h>

// UART Service UUID
BLEService uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

// TX Characteristic UUID
BLECharacteristic txCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite | BLENotify, 20);

// RX Characteristic UUID 
BLECharacteristic rxCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLEWrite, 20);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(LED_BUILTIN, OUTPUT);

  // 블루투스 초기화
  if (!BLE.begin()) {
    Serial.println("BLE 초기화 실패!");
    while (1);
  }

  // UART 서비스 및 특성 설정
  BLE.setLocalName("Arduino UART");
  BLE.setAdvertisedService(uartService);
  uartService.addCharacteristic(txCharacteristic);
  uartService.addCharacteristic(rxCharacteristic);
  BLE.addService(uartService);

  // 연결 대기
  BLE.advertise();
  Serial.println("BLE 장치 대기 중...");
}

void loop() {
  // 클라이언트 연결 대기
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("연결된 클라이언트: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      // TX Characteristic에 데이터 쓰기
      String txValue;
      for(int i = 0; i < 20; i++){
        txValue = String(i);
        txCharacteristic.writeValue(txValue.c_str(), txValue.length());
        Serial.println("TX 데이터 전송");
        //delay(200);
      }

      // RX Characteristic에서 데이터 읽기
      if (rxCharacteristic.written()) {
        uint8_t rxValue[20];
        rxCharacteristic.readValue(rxValue, sizeof(rxValue));
        Serial.print("RX 데이터 수신: ");
        for (int i = 0; i < sizeof(rxValue); i++) {
          Serial.print(rxValue[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }

      delay(1000);
    }
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("클라이언트 연결 해제");
  }
}