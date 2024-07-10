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
      // RX Characteristic에서 데이터 읽기
      if (rxCharacteristic.written()) {
        char rxValue[maxDataLength + 1]; // null terminator를 위한 공간 확보
        int length = rxCharacteristic.readValue(rxValue, maxDataLength);
        rxValue[length] = '\0'; // null terminator 추가

        // 수신된 데이터를 처리
        Serial.print("RX 데이터 수신: ");
        Serial.println(rxValue);

        // 문자열을 공백으로 분할
        char *token = strtok(rxValue, " ");
        dataCount = 0;
        
        while (token != NULL && dataCount < maxDataLength) {
          intArray[dataCount] = atoi(token); // 정수로 변환하여 배열에 저장
          dataCount++;
          token = strtok(NULL, " ");
        }

        // 배열에 저장된 데이터 출력
        Serial.println("저장된 int형 배열 데이터:");
        for (int i = 0; i < dataCount; i++) {
          Serial.println(intArray[i]);
        }
        Serial.println();
      }
      Serial.println(dataCount);

      delay(1000); // 데이터를 1초마다 전송 및 수신
    }
    
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("클라이언트 연결 해제");
  }
}
