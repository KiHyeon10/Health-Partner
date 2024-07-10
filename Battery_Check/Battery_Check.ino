#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_RST 6  // 디스플레이 리셋 핀
#define TFT_CS 5
#define TFT_DC 4
#define BATTERY_PIN A0  // 배터리 전압을 측정하는 아날로그 핀

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

#define BATTERY_FULL_VOLTAGE 3.7   // 배터리가 완전히 충전된 상태의 전압 (변경 가능)
#define BATTERY_EMPTY_VOLTAGE 0.0  // 배터리가 완전히 방전된 상태의 전압 (변경 가능)

#define BATTERY_GAUGE_X 140
#define BATTERY_GAUGE_Y 5
#define BATTERY_GAUGE_WIDTH 45
#define BATTERY_GAUGE_HEIGHT 15

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  pinMode(BATTERY_PIN, INPUT);
}
int Before_BatteryPersent = 1000;

void loop() {
  float batteryVoltage = readBatteryVoltage();
  Serial.print("batteryVolt: ");
  Serial.println(batteryVoltage);
  int batteryLevel = mapBatteryLevel(batteryVoltage);
  Serial.print("BatterLevel: ");
  Serial.println(batteryLevel);
  drawBatteryGauge(batteryLevel);
  delay(1000);  // 1초마다 갱신
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
  tft.drawRect(137, 1, 52, 21, ST77XX_RED);
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
    tft.fillRect(190, 5, 50, 15, ST77XX_BLACK);

    if (level == 100) {
      tft.setCursor(190, 5);
    } 
    else if (level < 100 && level > 9) {
      tft.setCursor(195, 5);
    } 
    else if (level < 10) {
      tft.setCursor(200, 5);
    }
    Before_BatteryPersent = level;

    tft.print(level);
    tft.println("%");
  }
}