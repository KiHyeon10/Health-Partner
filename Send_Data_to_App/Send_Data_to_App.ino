/*
#include <ArduinoBLE.h>

// UART Service UUID
BLEService uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

// TX Characteristic UUID
BLECharacteristic txCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite | BLENotify, 20);

// RX Characteristic UUID
BLECharacteristic rxCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLEWrite, 20);

const int maxDataLength = 20;
int intArray[maxDataLength];
int dataCount = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  pinMode(LED_BUILTIN, OUTPUT);

  // 블루투스 초기화
  if (!BLE.begin()) {
    Serial.println("BLE 초기화 실패!");
    while (1)
      ;
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
int bleflag = 0;
void loop() {
  // 클라이언트 연결 대기
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("연결된 클라이언트: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {

      int tx = 6;
      String txValue;
      txValue = String(tx);
      txCharacteristic.writeValue(txValue.c_str(), txValue.length());
      Serial.println("TX 데이터 전송");

      delay(1000);

      tx = 2;
      txValue = String(tx);
      txCharacteristic.writeValue(txValue.c_str(), txValue.length());
      Serial.println("TX 데이터 전송");

      delay(1000);
    }
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("클라이언트 연결 해제");
  }
}
*/

#include <ArduinoBLE.h>

// UART 서비스와 특성에 대한 UUID
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_CMD_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RESP_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEService uartService(SERVICE_UUID); // UART 서비스
BLECharacteristic cmdCharacteristic(CHARACTERISTIC_CMD_UUID, BLEWrite | BLEWriteWithoutResponse, 20); // 명령 특성
BLECharacteristic respCharacteristic(CHARACTERISTIC_RESP_UUID, BLERead | BLENotify, 20); // 응답 특성

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // BLE 초기화
  if (!BLE.begin()) {
    Serial.println("BLE 초기화 실패!");
    while (1);
  }

  // UART 서비스와 특성 설정
  BLE.setLocalName("Arduino UART");
  BLE.setAdvertisedService(uartService);
  uartService.addCharacteristic(cmdCharacteristic);
  uartService.addCharacteristic(respCharacteristic);
  BLE.addService(uartService);

  // 광고 시작
  BLE.advertise();
  Serial.println("BLE 장치가 연결을 기다리는 중...");
}

void loop() {
  // BLE 중앙 장치가 연결될 때까지 기다림
  BLEDevice central = BLE.central();

  // 중앙 장치가 연결되면
  if (central) {
    Serial.print("중앙 장치에 연결됨: ");
    Serial.println(central.address());

    // 중앙 장치가 연결된 동안
    while (central.connected()) {
      // 명령 특성이 작성되었는지 확인
      if (cmdCharacteristic.written()) {
        // 값 읽기
        const uint8_t* value = cmdCharacteristic.value();
        String cmd = String((const char*)value); // 수정된 부분
        Serial.print("받은 명령: ");
        Serial.println(cmd);

        // 사용자 입력을 위한 프롬프트
        Serial.print("응답을 입력하세요: ");
        while (!Serial.available()) {
          // 사용자 입력을 기다림
        }

        String response = Serial.readStringUntil('\n');
        Serial.print("응답 전송: ");
        Serial.println(response);

        // 사용자 입력을 응답으로 다시 전송
        respCharacteristic.writeValue(response.c_str(), response.length());
      }
    }

    // 중앙 장치가 연결 해제됨
    Serial.println("중앙 장치 연결 해제");
  }
}

