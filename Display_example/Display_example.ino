#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
//#include <Adafruit_ST7789_Fast.h>
#include <SPI.h>

#define TFT_RST 6  // 디스플레이 리셋 핀
#define TFT_CS 3
#define TFT_DC 2
#define SCREEN_WIDTH    240 // 디스플레이 너비
#define SCREEN_HEIGHT   240 // 디스플레이 높이

Adafruit_ST7789 lcd = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // 디스플레이 객체 생성

void setup() {
  Serial.begin(115200);
  lcd.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  lcd.setRotation(1);
}

void loop() {
  
  lcd.setTextSize(7);
  //lcd.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  lcd.fillScreen(ST77XX_BLACK);
  float total = 8;
  int x;
  for(int i = 0; i < 8; i++){
    //lcd.init(SCREEN_WIDTH, SCREEN_HEIGHT);
    if(i<9){
      x = 52;
    }
    else{
      x = 10;
    }
    lcd.setTextColor(ST77XX_WHITE);
    lcd.setCursor(x, 100);
    lcd.print("■");
    lcd.print('/');
    lcd.println((int)total);
    delay(300);

    lcd.setCursor(x, 100);
    lcd.setTextColor(ST77XX_BLACK);
    lcd.print(i+1);
  }
  delay(1000);
}

