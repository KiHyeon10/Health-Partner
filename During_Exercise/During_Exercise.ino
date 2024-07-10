#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_RST 6  // 디스플레이 리셋 핀
#define TFT_CS 3
#define TFT_DC 2
#define BATTERY_PIN A0  // 배터리 전압을 측정하는 아날로그 핀

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

#define BATTERY_FULL_VOLTAGE 5.0   // 배터리가 완전히 충전된 상태의 전압 (변경 가능)
#define BATTERY_EMPTY_VOLTAGE 0.0  // 배터리가 완전히 방전된 상태의 전압 (변경 가능)
//배터리 게이지바 위치 및 크
#define BATTERY_GAUGE_X 150
#define BATTERY_GAUGE_Y 5
#define BATTERY_GAUGE_WIDTH 45
#define BATTERY_GAUGE_HEIGHT 15

// 운동 게이지바의 위치 및 크기
#define EXERCISE_GAUGE_X 20
#define EXERCISE_GAUGE_Y 185
#define EXERCISE_GAUGE_WIDTH 210
#define EXERCISE_GAUGE_HEIGHT 20

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// 게이지바를 그리는 함수
void drawGaugeBar(float value, float total) {
  tft.fillRect(110, 215, 240, 30, ST77XX_BLACK);
  float gauge = 100 / total;
  float gauge_bar = gauge * value;
  tft.setTextSize(2);
  int textWidth = 5 * 3; // 폰트 크기에 따른 문자열 폭 추정
  int xPos = 190 - textWidth; // 출력을 시작할 x 위치 계산
  tft.setCursor(xPos, 215);
  float per = (value / total) * 100;
  if(per >= 100){
    tft.setCursor(175, 215);
    tft.print(per, 1);
    tft.println("%");
  }
  else{
    tft.print(per, 1);
    tft.println("%");
  }
  // 게이지바 바탕을 그립니다.
  //tft.fillRect(GAUGE_X, GAUGE_Y, GAUGE_WIDTH, GAUGE_HEIGHT, ST77XX_BLACK);
  // 게이지바를 그립니다.
  if(value <= total){
    float barWidth = map(gauge_bar, 0, 100, 0, EXERCISE_GAUGE_WIDTH); // 0부터 100 사이의 값을 게이지바의 너비로 변환
    tft.fillRect(EXERCISE_GAUGE_X, EXERCISE_GAUGE_Y, barWidth, EXERCISE_GAUGE_HEIGHT, ST77XX_GREEN);
  }
}

void setup() {
  Serial.begin(9600);
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  pinMode(BATTERY_PIN, INPUT);
}
//이전 배터리 용량
int Before_BatteryPersent = 1000;
//현재 운동 횟수
float value = 0;

void loop() {
  float total = 5;

  float batteryVoltage = readBatteryVoltage();
  Serial.print("batteryVolt: ");
  Serial.println(batteryVoltage);
  int batteryLevel = mapBatteryLevel(batteryVoltage);
  Serial.print("BatterLevel: ");
  Serial.println(batteryLevel);
  drawBatteryGauge(batteryLevel);
  Count_Exercise_set(&value, total);
  Serial.println(value); 
  
  // if(Altitude correct){

  // }

}
int flag = 0;

void Count_Exercise_set(float* value, float total){
  tft.drawRect(15, 177, 220, 36, ST77XX_RED);

  int x = 0;
  if(total != *value){
    tft.setTextSize(2);
    tft.setCursor(25, 50);
    tft.println("During Exercise...");

    if((*value) + 1 <10){
      x = 72;
    }
    else{
      if((*value) + 1 == 10){
        tft.setTextSize(7);
        tft.setCursor(72, 90);
        tft.setTextColor(ST77XX_BLACK);
        tft.println('9');
      }
      x = 30;
    }

    tft.setTextSize(7);
    tft.setCursor(x, 90);
    tft.setTextColor(ST77XX_BLACK);
    tft.println((int)(*value));

    (*value)++;
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(x, 90);
    tft.print((int)(*value));
    tft.print('/');
    tft.println((int)total);

    drawGaugeBar(*value, total);
    delay(300);
  }
  else{
    if(flag == 0){
      tft.setTextSize(2);
      tft.setCursor(25, 50);
      tft.setTextColor(ST77XX_BLACK);
      tft.println("During Exercise...");
      flag = 1;
    }

    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(42, 50);
    tft.println("Finish Exercise");
  }
}

float readBatteryVoltage() {
  int rawADC = analogRead(BATTERY_PIN);
  Serial.println(rawADC);
  float voltage = rawADC * (5.0 / 1023.0);  // 아날로그 핀의 전압을 측정하여 변환
  return voltage;
}

int mapBatteryLevel(float voltage) {
  // 배터리 전압을 범위로 매핑하여 배터리 잔량(%) 계산
  float batteryLevel = map(voltage, BATTERY_EMPTY_VOLTAGE, BATTERY_FULL_VOLTAGE, 0, 100);
  // 최소값과 최대값을 설정하여 반환
  return constrain(batteryLevel, 0, 100);
}

void drawBatteryGauge(int level) {
  //tft.fillRect(GAUGE_X, GAUGE_Y, GAUGE_WIDTH, GAUGE_HEIGHT, ST77XX_BLACK);
  tft.drawRect(147, 1, 52, 21, ST77XX_RED);
  int barWidth = map(level, 0, 100, 0, BATTERY_GAUGE_WIDTH);
  uint16_t fillColor = level > 20 ? ST77XX_GREEN : ST77XX_RED;  // 배터리 잔량이 20% 미만이면 빨간색으로 표시
  tft.fillRect(BATTERY_GAUGE_X, BATTERY_GAUGE_Y, barWidth, BATTERY_GAUGE_HEIGHT, fillColor);

  Write_BatteryPersent(level);
  Serial.print("battery persentage: ");
  Serial.println(level);
}

void Write_BatteryPersent(int level) {

  if (Before_BatteryPersent != level) {

    tft.setTextSize(2);
    tft.fillRect(200, 5, 50, 15, ST77XX_BLACK);

    if (level == 100) {
      tft.setCursor(200, 5);
    } 
    else if (level < 100 && level > 9) {
      tft.setCursor(205, 5);
    } 
    else if (level < 10) {
      tft.setCursor(210, 5);
    }
    Before_BatteryPersent = level;

    tft.print(level);
    tft.println("%");
  }
}