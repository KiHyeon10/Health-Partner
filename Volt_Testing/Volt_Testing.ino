#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>
#include <MPU9250.h>

#define TFT_RST 6  // 디스플레이 리셋 핀
#define TFT_CS 3
#define TFT_DC 2
#define Buzzer_Pin A1   //부저 핀
#define BUTTON D4       //버튼 핀

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

// 운동 게이지바의 위치 및 크기
#define EXERCISE_GAUGE_X 15
#define EXERCISE_GAUGE_Y 185
#define EXERCISE_GAUGE_WIDTH 210
#define EXERCISE_GAUGE_HEIGHT 20

//함수들 정리 시작

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

//함수들 정리 끝

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);  //LCD Display
MPU9250 mpu;                                                     //IMU센서

int loop_flag = 0;                 //현재 운동 횟수

// 필터링을 위한 변수들
float accZFiltered = 0.0;
float accZPrev = 0.0;
float accZPrevFiltered = 0.0;
float accZPrevPrevFiltered = 0.0;
float sum = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  while (!mpu.setup(0x68)) {  // MPU9250의 I2C 주소를 0x68로 설정
    Serial.println("MPU9250 초기화 실패. 연결을 확인하세요.");
    delay(5000);
  }
  Loading_Screen();
  Serial.println("Finish");
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  Waiting_Screen();

  Start_Exercise(0.0, 5.0);
}

//운동 시작 함수
void Start_Exercise(float value, float total) {
  while (value <= total) {
    Count_Exercise_set(&value, total);
    During_Exercise(&value);
    if (value == total) {
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
  float High = 0;
  float Low = 0;
  int Up_or_Down_flag = Starting_Timing();
  int During_flag = 0;

  if (Up_or_Down_flag == 1) {
    High = Setting_Up_Ver2();
    Low = Setting_Down_Ver2();
    During_flag = 1;
  } else if (Up_or_Down_flag == 2) {
    Low = Setting_Down_Ver2();
    High = Setting_Up_Ver2();
    During_flag = 1;
  }
  if (During_flag == 1) {
    (*value) += 1;
    High = 0;
    Low = 0;
    During_flag = 0;
    Serial.print("During Count: ");
    Serial.println(*value);
    Buzzer();
  }
}
int Timing_flag = 0;

//운동을 시작하는 타이밍인지 보는 함수
int Starting_Timing() {
  delay(100);
  float First_Height = Read_Height();
  Serial.println(First_Height);
  delay(50);
  float Second_Height = Read_Height();  // 오타 수정
  Serial.println(Second_Height);
  First_Height = Second_Height;
  while (First_Height >= Second_Height - 2.0 && First_Height <= Second_Height + 2.0) {
    First_Height = Read_Height();
  }
  if (First_Height > Second_Height) {
    return 1;
  } else if (First_Height < Second_Height) {
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
  Serial.println("Setting Up Start");

  float First_Height = Read_Height();
  //float First_Height = kalmanFilter(Read_Height());
  float High_Height = -100;
  int High_flag = 0;

  while (1) {
    if (High_Height <= First_Height) {
      High_flag = 0;
      High_Height = First_Height;
      Serial.print("High_Height: ");
      Serial.println(High_Height);
      delay(100);
      //First_Height = Read_Height();
    }
    if (High_flag >= 5) {
      break;
    }
    High_flag += 1;
    delay(100);
    //First_Height = Read_Height();
    First_Height = Read_Height();
  }
  Serial.println("Setting Up Finish");
  return abs(High_Height);
}

//최소 높이 측정하는 함수
float Setting_Down_Ver2() {
  Serial.println("Setting Down Start");

  float First_Height = Read_Height();
  //float First_Height = kalmanFilter(Read_Height());
  float Low_Height = 100;
  int Low_flag = 0;

  while (1) {
    if (Low_Height >= First_Height) {
      Low_flag = 0;
      Low_Height = First_Height;
      Serial.print("Low_Height: ");
      Serial.println(Low_Height);
      delay(150);
      //First_Height = Read_Height();
    }
    if (Low_flag >= 5) {
      break;
    }
    Low_flag += 1;
    delay(100);
    //First_Height = Read_Height();
    First_Height = Read_Height();
  }
  Serial.println("Setting Down Finish");
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
  float gauge = 100 / total;
  float gauge_bar = gauge * value;
  tft.setTextSize(2);
  int textWidth = 5 * 3;       // 폰트 크기에 따른 문자열 폭 추정
  int xPos = 190 - textWidth;  // 출력을 시작할 x 위치 계산
  tft.setCursor(xPos, 215);
  float per = (value / total) * 100;
  if (per >= 100) {
    tft.setCursor(160, 215);
    tft.print(per, 1);
    tft.println("%");
  } else {
    tft.print(per, 1);
    tft.println("%");
  }
  // 게이지바를 그립니다.
  if (value <= total) {
    float barWidth = (value / total) * EXERCISE_GAUGE_WIDTH;  // 0부터 100 사이의 값을 게이지바의 너비로 변환
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

    if ((*value) < 10) {
      x = 62;
    } else {
      if ((*value) == 10) {
        tft.setTextSize(7);
        tft.setCursor(62, 90);
        tft.setTextColor(ST77XX_BLACK);
        tft.println('9');
      }
      x = 20;
    }

    tft.setTextSize(7);
    tft.setCursor(x, 90);
    tft.setTextColor(ST77XX_BLACK);
    tft.println((int)(*value));

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
