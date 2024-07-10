#include <Arduino_LSM9DS1.h>
#include <iostream>
#include <Wire.h>
//#include <softTone.h>

using namespace std;

#define BuzzerPin 11
#define Button 6

bool loopFlag = true;
int startCount = 0;
float beforeAltitude = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Wire.begin();
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  pinMode(Button, INPUT);
  pinMode(BuzzerPin, OUTPUT);
}

void loop() {
  if (digitalRead(Button) == LOW && startCount == 5) {
    startCount = 0;
  }
  
  if (digitalRead(Button) == LOW && startCount == 0) {
    // Setting altitude range
    High_Low setAlt = SettingUpDown();
    startCount++;
    delay(1000);
  }
  
  else if (digitalRead(Button) == LOW && startCount < 5) {
    float exerciseAlt = 0;
    High_Low doAlt;
    int highFlag = 0;
    int lowFlag = 0;
    int count = 0;
    int tryCount = 0;
    
    Buzzer_Exercise();
    startCount++;
    
    while (loopFlag) {
      exerciseAlt = sendAltitude();
      
      if (beforeAltitude < exerciseAlt) {
        doAlt.High_Alt = exerciseAlt;
        highFlag = 1;
      }
      else if (beforeAltitude > exerciseAlt) {
        doAlt.Low_Alt = exerciseAlt;
        lowFlag = 1;
      }

      else if ((beforeAltitude <= exerciseAlt && highFlag == 1 && lowFlag == 1) ||
               (beforeAltitude >= exerciseAlt && lowFlag == 1 && highFlag == 1)) {
        if (abs(doAlt.High_Alt - doAlt.Low_Alt + 0.18) >= abs(setAlt.High_Alt - setAlt.Low_Alt)) {
          count++;
          tryCount++;
          Buzzer_Exercise();
        }
        else {
          tryCount++;
        }
        
        highFlag = 0;
        lowFlag = 0;
        doAlt.High_Alt = 0;
        doAlt.Low_Alt = 300;
        beforeAltitude = 0;
      }
      
      beforeAltitude = exerciseAlt;
      delay(1000);
    }
  }
}

void Buzzer() {
  //wiringPiSetupGpio();
  //softToneCreate(BuzzerPin);
  tone(BuzzerPin, 780);
  delay(50);
  tone(BuzzerPin, 0);
}

void Buzzer_Exercise() {
  //wiringPiSetupGpio();
  //softToneCreate(BuzzerPin);
  tone(BuzzerPin, 1244);
  delay(50);
  tone(BuzzerPin, 0);
}

class High_Low {
public:
  float High_Alt = 0;
  float Low_Alt = 300;
};

High_Low SettingUpDown() {
  Serial.println("Start Setting");
  High_Low setting;
  float altitude = 0;
  
  while (true) {
    Buzzer();
    delay(1000);
    altitude = sendAltitude();
    
    if (setting.High_Alt < altitude) {
      setting.High_Alt = altitude;
      Serial.println("Setting High: " + String(setting.High_Alt));
    }
    else if (setting.Low_Alt > altitude) {
      setting.Low_Alt = altitude;
      Serial.println("Setting Low: " + String(setting.Low_Alt));
    }
    else if (altitude < setting.High_Alt && altitude > setting.Low_Alt) {
      Serial.println("Setting finish");
      break;
    }
  }
  
  return setting;
}

bool FinishSet() {
  float alt = sendAltitude();
  float beforeAlt = sendAltitude();
  
  while (true) {
    alt = sendAltitude();
    Serial.println("Finish Checking");
    
    if (alt == beforeAlt) {
      startCount++;
    }
    else if (startCount >= 10) {
      startCount = 0;
      return false;
    }
    else {
      startCount = 0;
      return true;
    }
    
    beforeAlt = alt;
    delay(1000);
  }
}

float sendAltitude() {
  float altitude;
  int pressure, temp;
  
  temp = readTemperature();
  pressure = readPressure();
  altitude = readAltitude(0) + 200;
  
  if (altitude <= beforeAltitude + 0.5 && altitude >= beforeAltitude - 0.8) {
    return beforeAltitude;
  }
  else {
    beforeAltitude = altitude;
    return altitude;
  }
}

float readAltitude(int level) {
  float altitude = 0.0, pressure, seaLevelPressure;
  
  if (level == 0) seaLevelPressure = 101325.0;
  else seaLevelPressure = (float)level;
  
  pressure = readPressure();
  altitude = 44330.0 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
  
  return altitude;
}
