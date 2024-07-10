#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_RST 6  // 디스플레이 리셋 핀
#define TFT_CS 3
#define TFT_DC 2

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

void Loading_Screen();
void Draw_Dumbbell();
void Logo_Text();
void Loading_Text();
void Finish_Loading();

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


void setup() {
  Serial.begin(9600);
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  //tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ST77XX_RED);
  Loading_Screen();
}

void loop() {
}

void Loading_Screen(){
  Draw_Dumbbell();
  Logo_Text();
  Loading_Text();
}

void Draw_Dumbbell(){
  tft.fillRoundRect(40, 60, 20, 60, 2, ST77XX_WHITE);
  tft.fillRoundRect(65, 50, 25, 80, 3, ST77XX_WHITE);
  tft.drawRect(70, 80, 85, 20, ST77XX_WHITE);
  tft.fillRoundRect(180, 60, 20, 60, 2, ST77XX_WHITE);
  tft.fillRoundRect(150, 50, 25, 80, 3, ST77XX_WHITE);
}

void Logo_Text(){
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 140);
  tft.print("Health");
  tft.setCursor(100, 170);
  tft.print("Partner");
}

void Loading_Text(){
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);

  int x = 65;
  char Start_Text[7] = {'L', 'o', 'a', 'd', 'i', 'n', 'g'};

  for(int i = 0; i < 7; i++){
    tft.setCursor(x, 200);
    tft.print(Start_Text[i]);
    x += 10;
    delay(100);
  }

  for(int i = 0 ; i < 6; i++){
    if(i%2 == 0){
      tft.setTextColor(ST77XX_GREEN);
    }
    else{
      tft.setTextColor(ST77XX_BLACK);
    }
    for(int j = 0; j < 3; j++){
      tft.setCursor(x, 200);
      tft.print('.');
      x += 10;
      delay(100);
    }
    x -= 30;
  }

  Finish_Loading();
}

void Finish_Loading(){
  tft.setTextColor(ST77XX_BLACK);
  int x = 65;
  char Start_Text[7] = {'L', 'o', 'a', 'd', 'i', 'n', 'g'};

  for(int i = 0; i < 7; i++){
    tft.setCursor(x, 200);
    tft.print(Start_Text[i]);
    x += 10;
    delay(50);
  }
  tft.setCursor(75, 200);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("Welcome");
}