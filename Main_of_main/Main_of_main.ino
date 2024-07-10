#include "Arduino.h"
//디스플레이 관련 헤더 파일
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include <SPI.h>
#include <Wire.h>
//IMU센서 관련
#include <MPU9250.h>
//BLE관련
#include <ArduinoBLE.h>

#define TFT_RST 6  // 디스플레이 리셋 핀
#define TFT_CS 3
#define TFT_DC 2

#define Buzzer_Pin A1   //부저 핀
#define BUTTON A7       //버튼 핀

//디스플레이 최대 크기
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

// #define BATTERY_FULL_VOLTAGE 5.0   // 배터리가 완전히 충전된 상태의 전압 (변경 가능)
// #define BATTERY_EMPTY_VOLTAGE 0.0  // 배터리가 완전히 방전된 상태의 전압 (변경 가능)
// //배터리 게이지바 위치 및 크기
// #define BATTERY_GAUGE_X 145
// #define BATTERY_GAUGE_Y 5
// #define BATTERY_GAUGE_WIDTH 45
// #define BATTERY_GAUGE_HEIGHT 15

// 운동 게이지바의 위치 및 크기
#define EXERCISE_GAUGE_X 15
#define EXERCISE_GAUGE_Y 185
#define EXERCISE_GAUGE_WIDTH 210
#define EXERCISE_GAUGE_HEIGHT 20

// UART 서비스와 특성에 대한 UUID
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_CMD_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RESP_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

//함수들 정리 시작

void Receive_Data(String cmd);                                  //앱에서 아두이노로 데이터 수신하는 함수
void processCommand(String cmd);                                //받은 데이터 int형으로 처리하는 함수
void Send_Data(float value);                                    //아두이노에서 앱으로 데이터 송신하는 함수
void Start_Exercise(float value, float total);                  //운동 시작 함수
void Waiting_Screen();                                          //초기 화면
void Loading_Screen();                                          //대기 화면
void During_Exercise(float count);                              //운동할 때 횟수 측정하는 함수
int Starting_Timing();                                          //운동을 시작하는 타이밍인지 보는 함수
void Buzzer();                                                  //부저 울리는 함수
float Read_Height();                                            //기속도 측정하는 함수
float Calc_Height(float accZFiltered, float accZPrevFiltered);  //가속도값을 높이로 계산하는 함수
float Setting_Up_Ver2();                                        //최고 높이 측정하는 함수
float Setting_Down_Ver2();                                      //최소 높이 측정하는 함수
void Draw_Dumbbell();                                           //덤벨모양 만들기
void Logo_Text();                                               //Health Partner 로고 보여주기
void Loading_Text();                                            //초기 화면에 로딩중이라고 보여주기
void Finish_Loading();                                          //로딩이 끝나면 welcome으로 바꿔주기
void drawGaugeBar(float value, float total);                    //운동 게이지바를 그리는 함수
void Count_Exercise_set(float* value, float total);             //운동 횟수 그리는 함수
// float readBatteryVoltage();                                     //배터리 전압 측정 함수
// int mapBatteryLevel(float voltage);                             //배터리 잔량 계산 함수
// void drawBatteryGauge(int level);                               //배터리 게이지 그리는 함수
// void Write_BatteryPersent(int level);                           //배터리 잔량 표시 함수

//함수들 정리 끝

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);  //LCD Display
MPU9250 mpu;                                                     //IMU센서

BLEService uartService(SERVICE_UUID);                                                                  // UART 서비스
BLECharacteristic cmdCharacteristic(CHARACTERISTIC_CMD_UUID, BLEWrite | BLEWriteWithoutResponse, 20);  // 명령 특성
BLECharacteristic respCharacteristic(CHARACTERISTIC_RESP_UUID, BLERead | BLENotify, 20);               // 응답 특성

// int Before_BatteryPersent = 1000;  //이전 배터리 용량
int loop_flag = 0;                 //현재 운동 횟수
float sum = 0;
const int maxDataLength = 20;   //블루투스로 받을 수 있는 최대 개수
int intArray[maxDataLength];  // 블루투스로 받은 값 string -> int로 변경 후 저장할 배열
int dataCount = 0;            // intArray 배열에 저장된 데이터 개수
String commandBuffer = "";    // 블루투스로 받은 값을 일시적으로 저장할 버퍼

// 필터링을 위한 변수들
float accZFiltered = 0.0; //이전 imu 측정 값
float accZPrev = 0.0;     //현재 imu 측정 값
float accZPrevFiltered = 0.0; //필터링한 이전 imu 측정 값
float accZPrevPrevFiltered = 0.0; //필터링한 이전 이전 imut 측정 값

void setup() {
  //Serial.begin(115200);
  tft.init(TFT_WIDTH, TFT_HEIGHT);  //디스플레이 초기화
  tft.setRotation(3); //디스플레이 회전
  tft.fillScreen(ST77XX_BLACK); //디스플레이 검은색으로 채우기

  Wire.begin();
  // pinMode(BATTERY_PIN, INPUT);
  pinMode(BUTTON, INPUT); //버튼 input으로 하기

  while (!mpu.setup(0x68)) {  // MPU9250의 I2C 주소를 0x68로 설정
      // Serial.println("MPU9250 초기화 실패");
      delay(5000);
  }

  // BLE 초기화
  while (!BLE.begin()) {
    // Serial.println("BLE 초기화 실패!");
  }

  // UART 서비스와 특성 설정
  BLE.setLocalName("Arduino UART"); //아두이노 블루투스 이름 Arduino UART로 하기
  BLE.setAdvertisedService(uartService);
  uartService.addCharacteristic(cmdCharacteristic);
  uartService.addCharacteristic(respCharacteristic);
  BLE.addService(uartService);

  // 블루투스 scanning
  BLE.advertise();
  // Serial.println("BLE 장치가 연결을 기다리는 중...");

  //tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ST77XX_RED);
  Loading_Screen(); //블루투스 연결될 동안 로딩화면 보여주기
}

void loop() {
  // 블루투스 연결될 때까지 기다림
  BLEDevice central = BLE.central();
  if (central) {
    // Serial.print("중앙 장치에 연결됨: ");
    // Serial.println(central.address());

    tft.fillScreen(ST77XX_BLACK);
    Waiting_Screen();

    // 중앙 장치가 연결된 동안
    while (central.connected()) {
      // 명령 특성이 작성되었는지 확인
      if (cmdCharacteristic.written()) {
        // 블루투스로 받은 값 읽기
        const uint8_t* value = cmdCharacteristic.value();
        int length = cmdCharacteristic.valueLength();
        String cmd = "";
        for (int i = 0; i < length; i++) {
          cmd += (char)value[i];
        }
        Receive_Data(cmd);

        //받은 값이 있으면 운동 시작
        if (length != 0) {
          int i = 0;
          int Button_flag = 0;
          float Exercise_count = 0; //운동 횟수 카운팅
          int Finish_flag = 0;
          //Serial.println(digitalRead(BUTTON));

          //한 운동 당 세트 수 4개 이하로 세팅
          while (i < 4) {
            //버튼이 눌렸을 경우 운동을 시작하는 걸로 판단
            if (Button_flag == 0 && digitalRead(BUTTON) == HIGH) {
              //delay(50);
              if (digitalRead(BUTTON) == HIGH) {  //버튼 바운딩 에러로 한 번 더 확인
                tft.fillScreen(ST77XX_BLACK);
                Start_Exercise(Exercise_count, (float)intArray[i]);
                Button_flag = 1;
              }
              i += 1;
            }
            //tft.fillScreen(ST77XX_BLACK);
            while (Button_flag == 1) {
              //운동 횟수 측정이 끝나면 앱으로 횟수 전송
              if (Finish_flag == 0) {
                Send_Data(intArray[i]);
                Finish_flag = 1;
              } else {
                //버튼이 다시 눌렸을 경우(세트 종료) 대기 화면으로 이동
                if (digitalRead(BUTTON) != HIGH) {
                  Waiting_Screen();
                } else {
                  Button_flag = 0;
                  Finish_flag = 0;
                }
              }
            }
          }
          length = 0;
        }
      }
    }

    // 중앙 장치가 연결 해제됨
    //Serial.println("중앙 장치 연결 해제");
  }

  //배터리 잔량 표시
  // float batteryVoltage = readBatteryVoltage();
  // int batteryLevel = mapBatteryLevel(batteryVoltage);
  // drawBatteryGauge(batteryLevel);

  // if (loop_flag == 0) {
  //   Waiting_Screen();
  //   delay(500);
  //   loop_flag = 1;
  // }

  // else if (loop_flag == 1) {
  //   float total = 5;
  //   float value = 0;
  //   Start_Exercise(value, total);
  //   loop_flag = 0;
  //   delay(500);
  // }
  // tft.fillScreen(ST77XX_BLACK);
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
  // Serial.print("받은 명령: ");
  // Serial.println(cmd);

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
  // Serial.print("int 배열: ");
  // for (int i = 0; i < dataCount; i++) {
  //   Serial.print(intArray[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();
}

//아두이노에서 앱으로 데이터 송신하는 함수
void Send_Data(int value) {
  String data = String(value);
  // Serial.print("응답 전송: ");
  // Serial.println(data);

  // 변수의 값을 응답으로 전송
  respCharacteristic.writeValue(data.c_str(), data.length());
}

//운동 시작 함수
void Start_Exercise(float value, float total) {
  while (value <= total) { //아직 운동이 안 끝남
    Count_Exercise_set(&value, total);
    During_Exercise(&value);
    if (value == total) { //마지막 횟수일 때
      Count_Exercise_set(&value, total);
      break;
    }
  }
}

//초기 화면
void Loading_Screen() {
  Draw_Dumbbell();
  Logo_Text();
  Loading_Text();
}

//대기 화면
void Waiting_Screen() {
  Draw_Dumbbell();
  Logo_Text();
}

//운동할 때 횟수 측정하는 함수
void During_Exercise(float* value) {
  float High = 0; //제품을 올릴 때
  float Low = 0;  //제품을 내릴 
  int Up_or_Down_flag = Starting_Timing(); //운동 시작 타이밍을 알려줌
  int During_flag = 0;

  //제품이 올라갔다가 내려가는게 횟수 1 일 때
  if (Up_or_Down_flag == 1) {
    High = Setting_Up_Ver2();
    Low = Setting_Down_Ver2();
    During_flag = 1;
  }
  //제품이 내려갔다가 올라가는게 횟수 1 일 때
  else if (Up_or_Down_flag == 2) {
    Low = Setting_Down_Ver2();
    High = Setting_Up_Ver2();
    During_flag = 1;
  }
  //횟수 1를 채웠을 때
  if (During_flag == 1) {
    (*value) += 1;
    High = 0;
    Low = 0;
    During_flag = 0;
    // Serial.print("During Count: ");
    // Serial.println(*value);
    Buzzer();
  }
}

int Timing_flag = 0;

//운동을 시작하는 타이밍인지 보는 함수
int Starting_Timing() {
  delay(100);
  float First_Height = Read_Height(); //현재 imu 측정값
  // Serial.println(First_Height);
  delay(50);
  float Seconed_Height = Read_Height(); //이전 imu측정값
  // Serial.println(Seconed_Height);
  First_Height = Seconed_Height;
  
  //First_Height와 Seconed_Height의 차이가 ±2일 경우 운동을 시작한게 아니라고 판단
  while (First_Height >= Seconed_Height - 2.0 && First_Height <= Seconed_Height + 2.0) {
    First_Height = Read_Height();
  }
  //First_Height가 더 클 경우 제품이 올라갔다가 내려가는게 횟수 1이라고 판단
  if (First_Height > Seconed_Height) {
    return 1;
  }
  //Seconed_Height가 더 클 경우 제품이 내려갔다가 올라가는게 횟수 1이라고 판단
  else if (First_Height < Seconed_Height) {
    return 2;
  } else {
    return 0;
  }
}

//부저 울리는 함수
void Buzzer() {
  tone(Buzzer_Pin, 524);
  delay(100);
  noTone(Buzzer_Pin);
}

//기속도 측정하는 함수
float Read_Height() {
  if (mpu.update()) {
    float accZ = mpu.getAccZ();  // 가속도 Z값 읽어오기
    //Serial.print("Z: ");dfdfdfd
    //Serial.println(accZ);

    // 2차 저역통과 필터 적용
    accZFiltered = 0.6 * accZ + 0.2 * accZPrev - 0.2 * accZPrevPrevFiltered + 0.3 * accZPrevFiltered;

    // 이전 값 업데이트
    //Serial.println(accZFiltered);
    //Serial.println(accZPrevFiltered);
    float Height = Calc_Height(accZFiltered, accZPrevFiltered);
    sum += Height;
    accZPrevPrevFiltered = accZPrevFiltered;
    accZPrevFiltered = accZFiltered;
    accZPrev = accZ;

    return sum;
  }
}

//가속도값을 높이로 계산하는 함수
float Calc_Height(float accZFiltered, float accZPrevFiltered) {
  float Height = 0;
  Height = (accZFiltered - accZPrevFiltered) * 9;

  return Height;
}

//최고 높이 측정하는 함수
float Setting_Up_Ver2() {
  //Serial.println("Setting Up Start");

  float First_Height = Read_Height();
  //float First_Height = kalmanFilter(Read_Height());
  float High_Height = -100;
  int High_flag = 0;

  while (1) {
    if (High_Height <= First_Height) {
      High_flag = 0;
      High_Height = First_Height;
      // Serial.print("High_Height: ");
      // Serial.println(High_Height);
      delay(100);
      //First_Height = Read_Height();
    }
    //제품이 위로 올라가는 지 5번 확인
    if (High_flag >= 5) {
      break;
    }
    High_flag += 1;
    delay(100);
    //First_Height = Read_Height();
    First_Height = Read_Height();
  }
  //Serial.println("Setting Up Finish");
  return abs(High_Height);
}

//최소 높이 측정하는 함수
float Setting_Down_Ver2() {
  //Serial.println("Setting Down Start");

  float First_Height = Read_Height();
  //float First_Height = kalmanFilter(Read_Height());
  float Low_Height = 100;
  int Low_flag = 0;

  while (1) {
    if (Low_Height >= First_Height) {
      Low_flag = 0;
      Low_Height = First_Height;
      //Serial.print("Low_Height: ");
      //Serial.println(Low_Height);
      delay(150);
      //First_Height = Read_Height();
    }
    //제품이 내려가는 지 5번 확인
    if (Low_flag >= 5) {
      break;
    }
    Low_flag += 1;
    delay(100);
    //First_Height = Read_Height();
    First_Height = Read_Height();
  }
  //Serial.println("Setting Down Finish");
  return abs(Low_Height);
}

//덤벨모양 만들기
void Draw_Dumbbell() {
  tft.fillRoundRect(40, 60, 20, 60, 2, ST77XX_WHITE);
  tft.fillRoundRect(65, 50, 25, 80, 3, ST77XX_WHITE);
  tft.drawRect(70, 80, 85, 20, ST77XX_WHITE);
  tft.fillRoundRect(180, 60, 20, 60, 2, ST77XX_WHITE);
  tft.fillRoundRect(150, 50, 25, 80, 3, ST77XX_WHITE);
}

//Health Partner 로고 보여주기
void Logo_Text() {
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 140);
  tft.print("Health");
  tft.setCursor(100, 170);
  tft.print("Partner");
}

//초기 화면에 로딩중이라고 보여주기
void Loading_Text() {
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);

  int x = 65;
  char Start_Text[7] = { 'L', 'o', 'a', 'd', 'i', 'n', 'g' };

  for (int i = 0; i < 7; i++) {
    tft.setCursor(x, 200);
    tft.print(Start_Text[i]);
    x += 10;
    delay(100);
  }

  for (int i = 0; i < 6; i++) {
    if (i % 2 == 0) {
      tft.setTextColor(ST77XX_GREEN);
    } else {
      tft.setTextColor(ST77XX_BLACK);
    }
    for (int j = 0; j < 3; j++) {
      tft.setCursor(x, 200);
      tft.print('.');
      x += 10;
      delay(100);
    }
    x -= 30;
  }

  Finish_Loading();
}

//로딩이 끝나면 welcome으로 바꿔주기
void Finish_Loading() {
  tft.setTextColor(ST77XX_BLACK);
  int x = 65;
  char Start_Text[7] = { 'L', 'o', 'a', 'd', 'i', 'n', 'g' };

  for (int i = 0; i < 7; i++) {
    tft.setCursor(x, 200);
    tft.print(Start_Text[i]);
    x += 10;
    delay(50);
  }
  tft.setCursor(75, 200);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("Welcome");
}

// 운동 게이지바를 그리는 함수
void drawGaugeBar(float value, float total) {
  tft.fillRect(110, 215, 240, 30, ST77XX_BLACK);
  float gauge = 100 / total; //횟수 1회당 얼마나 채워야하는 지 계산
  float gauge_bar = gauge * value;
  tft.setTextSize(2);
  int textWidth = 5 * 3;       // 폰트 크기에 따른 문자열 폭 추정
  int xPos = 190 - textWidth;  // 출력을 시작할 x 위치 계산
  tft.setCursor(xPos, 215);
  float per = (value / total) * 100;
  if (per >= 100) { //total 횟수 채운 후 운동을 더 할 경우
    tft.setCursor(160, 215);
    tft.print(per, 1);
    tft.println("%");
  }
  else {
    tft.print(per, 1);
    tft.println("%");
  }
  // 게이지바를 그립니다.
  if (value <= total) {
    float barWidth = map(gauge_bar, 0, 100, 0, EXERCISE_GAUGE_WIDTH);  // 0부터 100 사이의 값을 게이지바의 너비로 변환
    tft.fillRect(EXERCISE_GAUGE_X, EXERCISE_GAUGE_Y, barWidth, EXERCISE_GAUGE_HEIGHT, ST77XX_GREEN);
  }
}

int flag = 0;

//운동 횟수 그리는 함수
void Count_Exercise_set(float* value, float total) {
  tft.drawRect(10, 177, 220, 36, ST77XX_RED);

  int x = 0;
  if (total >= *value) {
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(15, 50);
    tft.println("During Exercise...");

    if ((*value) < 10) { //운동 횟수를 10개 미만으로 했을 때
      x = 62;
    }
    else {
      //운동 횟수를 10개 했을 때
      if ((*value) == 10) {
        tft.setTextSize(7);
        tft.setCursor(62, 90);
        //운동 횟수 9가 사라지지 않아 하드코딩으로 없애줌.
        tft.setTextColor(ST77XX_BLACK);
        tft.println('9');
      }
      x = 20;
    }

    tft.setTextSize(7);
    tft.setCursor(x, 90);
    tft.setTextColor(ST77XX_BLACK);
    tft.println((int)(*value) - 1);

    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(x, 90);
    tft.print((int)(*value));
    tft.print('/');
    tft.println((int)total);

    drawGaugeBar(*value, total);
    if (total == *value) {
      flag = 1;
    }
  }
  //목표한 횟수를 다 채울 시, "Finsih Exercise"를 출력
  if (flag == 1) {
    tft.setTextSize(2);
    tft.setCursor(15, 50);
    tft.setTextColor(ST77XX_BLACK);
    tft.println("During Exercise...");
    flag = 0;

    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(32, 50);
    tft.println("Finish Exercise");
  }
}

// //배터리 전압 측정 함수
// float readBatteryVoltage() {
//   int rawADC = analogRead(BATTERY_PIN);
//   Serial.println(rawADC);
//   float voltage = rawADC * (5.0 / 1023.0);  // 아날로그 핀의 전압을 측정하여 변환
//   return voltage;
// }

// //배터리 잔량 계산 함수
// int mapBatteryLevel(float voltage) {
//   // 배터리 전압을 범위로 매핑하여 배터리 잔량(%) 계산
//   float batteryLevel = map(voltage, BATTERY_EMPTY_VOLTAGE, BATTERY_FULL_VOLTAGE, 0, 100);
//   // 최소값과 최대값을 설정하여 반환
//   return constrain(batteryLevel, 0, 100);
// }

// //배터리 게이지 그리는 함수
// void drawBatteryGauge(int level) {
//   //tft.fillRect(GAUGE_X, GAUGE_Y, GAUGE_WIDTH, GAUGE_HEIGHT, ST77XX_BLACK);
//   tft.drawRect(142, 1, 52, 21, ST77XX_RED);
//   int barWidth = map(level, 0, 100, 0, BATTERY_GAUGE_WIDTH);
//   uint16_t fillColor = level > 20 ? ST77XX_GREEN : ST77XX_RED;  // 배터리 잔량이 20% 미만이면 빨간색으로 표시
//   tft.fillRect(BATTERY_GAUGE_X, BATTERY_GAUGE_Y, barWidth, BATTERY_GAUGE_HEIGHT, fillColor);

//   Write_BatteryPersent(level);
//   Serial.print("battery persentage: ");
//   Serial.println(level);
// }

// //배터리 잔량 표시 함수
// void Write_BatteryPersent(int level) {

//   if (Before_BatteryPersent != level) {

//     tft.setTextSize(2);
//     tft.fillRect(195, 5, 50, 15, ST77XX_BLACK);

//     if (level == 100) {
//       tft.setCursor(195, 5);
//     } else if (level < 100 && level > 9) {
//       tft.setCursor(200, 5);
//     } else if (level < 10) {
//       tft.setCursor(205, 5);
//     }
//     Before_BatteryPersent = level;

//     tft.print(level);
//     tft.println("%");
//   }
// }
