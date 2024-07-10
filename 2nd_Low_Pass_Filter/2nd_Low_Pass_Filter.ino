#include <Wire.h>
#include <MPU9250.h>

#define Buzzer_Pin A0

MPU9250 mpu;

// 필터링을 위한 변수들
float accZFiltered = 0.0;
float accZPrev = 0.0;
float accZPrevFiltered = 0.0;
float accZPrevPrevFiltered = 0.0;

float sum = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.setup(0x68)) {  // MPU9250의 I2C 주소를 0x68로 설정
    while (1) {
      Serial.println("MPU9250 초기화 실패");
      delay(5000);
    }
  }
}

void Buzzer() {
  tone(Buzzer_Pin, 524);
  delay(100);
  noTone(Buzzer_Pin);
}

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
    accZPrevPrevFiltered = accZPrevFiltered;
    accZPrevFiltered = accZFiltered;
    accZPrev = accZ;

    return Height;
  }
}

float Calc_Height(float accZFiltered, float accZPrevFiltered) {
  float Height = 0;
  Height = (accZFiltered - accZPrevFiltered) * 9;

  return Height;
}

int val = 0;
float Setting_Up_Ver2() {
  Serial.println("Setting Up Start");

  float First_Height = 0;
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
    delay(200);
    //First_Height = Read_Height();
    First_Height = Read_Height();
  }
  Serial.println("Setting Up Finish");
  return abs(High_Height);
}

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
      delay(100);
      //First_Height = Read_Height();
    }
    if (Low_flag >= 5) {
      break;
    }
    Low_flag += 1;
    delay(200);
    //First_Height = Read_Height();
    First_Height = Read_Height();
  }
  Serial.println("Setting Down Finish");
  return abs(Low_Height);
}

int During_Exercise(float Height_Dif) {
  float First_Height = Read_Height();
  delay(200);
  float Seconed_Height = Read_Height();
  float High = 0;
  float Low = 0;
  int count = 0;
  int Up_or_Down_flag = 0;
  int During_flag = 0;

  while (First_Height == Seconed_Height) {
    First_Height = Read_Height();
  }

  if (First_Height > Seconed_Height) {
    Up_or_Down_flag = 1;
  } else if (First_Height < Seconed_Height) {
    Up_or_Down_flag = 2;
  }

  while (count < 5) {
    if (Up_or_Down_flag == 1) {
      High = Setting_Up_Ver2();
      Low = Setting_Down_Ver2();
      During_flag = 1;
    } else if (Up_or_Down_flag == 2) {
      Low = Setting_Down_Ver2();
      High = Setting_Up_Ver2();
      During_flag = 1;
    }
    if (abs(High - Low) != 0 && abs(High - Low) >= Height_Dif && During_flag == 1) {
      count += 1;
      High = 0;
      Low = 0;
      During_flag = 0;
      Serial.print("During Count: ");
      Serial.println(count);
      Buzzer();
    }
  }
  return count;
}

int mmm_flag = 0;

void loop() {

  if (mmm_flag == 0) {
    Serial.println("Start");
    delay(1000);
    float High_Hei = Setting_Up_Ver2();
    delay(1500);
    float Low_Hei = Setting_Down_Ver2();

    float Height_Dif = abs(High_Hei - Low_Hei - 4.1);
    mmm_flag = 1;
    Serial.print("Height_Dif: ");
    Serial.println(Height_Dif);
    int count = During_Exercise(Height_Dif);
    Serial.print("Count: ");
    Serial.println(count);
  } else {
    while (1) {
      delay(10000);
    }
  }
  delay(500);
}
