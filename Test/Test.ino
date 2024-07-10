#include "MPU9250.h"

MPU9250 mpu;
#define Buzzer_Pin A0

// 저역통과 필터 상수
const float alpha = 0.78;  // 필터 강도

// 이전 측정값 초기화
float prevAccZ = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);
  pinMode(Buzzer_Pin, OUTPUT);

  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }

  //print_calibration();
  mpu.verbose(false);
}

// 저역통과 필터 함수 정의
float lowPassFilter(float newValue, float oldValue, float alpha) {
  return alpha * newValue + (1 - alpha) * oldValue;
}

void Buzzer() {
  tone(Buzzer_Pin, 524);
  delay(100);
  noTone(Buzzer_Pin);
}

float Read_Height() {
  // MPU 업데이트
  if (mpu.update()) {
    // 현재 측정값 읽기
    float currentAccZ = mpu.getAccZ();

    // 저역통과 필터 적용
    float filteredAccZ = lowPassFilter(currentAccZ, prevAccZ, alpha);

    // 결과 출력
    //Serial.print("Filtered AccZ: ");
    //Serial.println(filteredAccZ);
    //Serial.print("Z: ");
    //Serial.println(currentAccZ);

    // 이전 측정값 업데이트
    prevAccZ = filteredAccZ;

    return filteredAccZ;
  }
}

float Setting_Up() {
  Serial.println("Setting Up Start");

  //float First_Height = Read_Height();
  float First_Height = Read_Height();
  float High_Height = -100;

  while (High_Height <= First_Height) {
    High_Height = First_Height;
    Serial.print("High_Height: ");
    Serial.println(High_Height);
    delay(500);
    //First_Height = Read_Height();
    First_Height = Read_Height();
  }
  Serial.println("Setting Up Finish");
  return abs(High_Height);
}

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
      delay(300);
      //First_Height = Read_Height();
    }
    if (High_flag >= 2) {
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
  float fuck = First_Height;
  float Low_Height = 100;
  int Low_flag = 0;

  while (1) {
    if (Low_Height >= First_Height) {
      Low_flag = 0;
      Low_Height = First_Height;
      Serial.print("Low_Height: ");
      Serial.println(Low_Height);
      delay(300);
      //First_Height = Read_Height();
    }
    if (Low_flag >= 3) {
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

int During_Exercise(float High_Height, float Low_Height) {
  float First_Height = Read_Height();
  delay(100);
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
    if (abs(High - Low) != 0 && abs(High - Low) >= abs(High_Height - Low_Height) && During_flag == 1) {
      count += 1;
      High = 0;
      Low = 0;
      During_flag = 0;
      Serial.print("During Count: ");
      Serial.println(count);
      //Buzzer();
    }
  }
  return count;
}

int mmm_flag = 0;

void loop() {
  !mpu.update();

  if (mmm_flag == 0) {
    Serial.println("Start");
    //delay(5000);
    float Height_Hei = Setting_Up_Ver2();
    delay(1500);
    float Low_Hei = Setting_Down_Ver2();
    mmm_flag = 1;
    Serial.print("Height_Dif: ");
    Serial.println(Height_Hei - Low_Hei);
    int count = During_Exercise(Height_Hei, Low_Hei);
    Serial.print("Count: ");
    Serial.println(count);
  } else {
    while (1) {
      delay(10000);
    }
  }
  delay(500);
}
