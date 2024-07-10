#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>
#include <MPU9250.h>
#include <ArduinoBLE.h>

#define TFT_RST 6  // 디스플레이 리셋 핀
#define TFT_CS 3
#define TFT_DC 2
#define BATTERY_PIN A0  // 배터리 전압을 측정하는 아날로그 핀
#define Buzzer_Pin A1   //부저 핀
#define BUTTON D4       //버튼 핀

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

// UART 서비스와 특성에 대한 UUID
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_CMD_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RESP_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEService uartService(SERVICE_UUID);                                                                  // UART 서비스
BLECharacteristic cmdCharacteristic(CHARACTERISTIC_CMD_UUID, BLEWrite | BLEWriteWithoutResponse, 20);  // 명령 특성
BLECharacteristic respCharacteristic(CHARACTERISTIC_RESP_UUID, BLERead | BLENotify, 20);               // 응답 특성

int Before_BatteryPersent = 1000;  //이전 배터리 용량
int loop_flag = 0;                 //현재 운동 횟수
float sum = 0;
const int maxDataLength = 20;
int intArray[maxDataLength];  // int형 배열
int dataCount = 0;            // 배열에 저장된 데이터 개수
String commandBuffer = "";    // 명령을 일시적으로 저장할 버퍼


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);  //LCD Display

void setup() {
  Serial.begin(9600);
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  Wire.begin();
  pinMode(BATTERY_PIN, INPUT);
  pinMode(BUTTON, INPUT_PULLDOWN);

  // BUTTON 핀 초기화
  while (digitalRead(BUTTON) == HIGH) {
    delay(50);
  }

  // BLE 초기화
  if (!BLE.begin()) {
    Serial.println("BLE 초기화 실패!");
    while (1)
      ;
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

  //tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ST77XX_RED);
}

void loop() {
  // BLE 중앙 장치가 연결될 때까지 기다림
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("중앙 장치에 연결됨: ");
    Serial.println(central.address());

    // 중앙 장치가 연결된 동안
    while (central.connected()) {
      // 명령 특성이 작성되었는지 확인
      if (cmdCharacteristic.written()) {
        // 값 읽기
        const uint8_t* value = cmdCharacteristic.value();
        int length = cmdCharacteristic.valueLength();
        String cmd = "";
        for (int i = 0; i < length; i++) {
          cmd += (char)value[i];
        }
        Receive_Data(cmd);

        Serial.print("Length: ");
        Serial.println(length);

        for(int i = 1; i < 5; i++) {
          Send_Data(i);
          Serial.print("Send: ");
          Serial.println(i);
          delay(200);
        }
      
      }
    }
     // 중앙 장치가 연결 해제됨
  Serial.println("중앙 장치 연결 해제");
  }
}


//앱에서 아두이노로 데이터 수신하는 함수
void Receive_Data(String cmd) {
  for (int i = 0; i < cmd.length(); i++) {
    char c = cmd[i];
    if (c == '\n') {
      // 명령의 끝을 인식하면 명령 처리
      processCommand(commandBuffer);
      commandBuffer = "";  // 버퍼 초기화
    } else {
      // 버퍼에 추가
      commandBuffer += c;
    }
  }
}

//받은 데이터 int형으로 처리하는 함수
void processCommand(String cmd) {
  Serial.print("받은 명령: ");
  Serial.println(cmd);

  // String 값을 int 배열로 변환
  dataCount = 0;
  int startIndex = 0;
  for (int i = 0; i < cmd.length(); i++) {
    if (cmd[i] == ' ' || i == cmd.length() - 1) {
      if (i == cmd.length() - 1) {
        i++;  // 마지막 숫자를 포함시키기 위해
      }
      String numStr = cmd.substring(startIndex, i);
      int num = numStr.toInt();
      if (dataCount < maxDataLength) {
        intArray[dataCount] = num;
        dataCount++;
      }
      startIndex = i + 1;
    }
  }
  // 변환된 int 배열 출력
  Serial.print("int 배열: ");
  for (int i = 0; i < dataCount; i++) {
    Serial.print(intArray[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("int length: ");
  Serial.println(dataCount);
}

//아두이노에서 앱으로 데이터 송신하는 함수
void Send_Data(int value) {
  String data = String(value);
  Serial.print("응답 전송: ");
  Serial.println(data);

  // 변수의 값을 응답으로 전송
  respCharacteristic.writeValue(data.c_str(), data.length());
}
