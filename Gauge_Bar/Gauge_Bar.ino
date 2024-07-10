#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_RST         6 // 디스플레이 리셋 핀
#define TFT_CS          5
#define TFT_DC          4
// TFT LCD의 크기
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// 게이지바의 위치 및 크기
#define EXERCISE_GAUGE_X 10
#define EXERCISE_GAUGE_Y 120
#define EXERCISE_GAUGE_WIDTH 210
#define EXERCISE_GAUGE_HEIGHT 20

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // 디스플레이 객체 생성

// 게이지바를 그리는 함수
void drawGaugeBar(float value, float total) {
  tft.fillRect(100, 150, 240, 30, ST77XX_BLACK);
  float gauge = 100 / total;
  float gauge_bar = gauge * value;
  tft.setTextSize(2);
  int textWidth = 5 * 3; // 폰트 크기에 따른 문자열 폭 추정
  int xPos = 190 - textWidth; // 출력을 시작할 x 위치 계산
  tft.setCursor(xPos, 150);
  float per = (value / total) * 100;
  if(per >= 100){
    tft.setCursor(165, 150);
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
  // TFT LCD 초기화
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(1); // 화면 회전 설정
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  float count = 12;
  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.println("During Exercise...");
  float x = 0;

  // 1%부터 100%까지 채워지는 게이지바를 표시합니다.
  tft.drawRect(5, 112, 220, 36, ST77XX_RED);
  while(x <= 20){
    drawGaugeBar(x, count);
    x += 1;
    delay(300);
  }
  tft.fillRect(0, 80, 240, 30, ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, 80);
  tft.println("Finish Exercise");
  delay(1000); // 게이지바가 완전히 채워진 후 1초 대기
}
