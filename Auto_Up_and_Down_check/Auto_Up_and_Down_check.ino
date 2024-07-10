#include "MPU9250.h"

MPU9250 mpu;


// 칼만 필터 변수
float P = 0.5;      // 추정 오차 공분산
float Q = 0.5;      // 프로세스 노이즈 공분산
float R = 1.0;      // 관측 노이즈 공분산
float K;            // 칼만 이득
float X = 0;        // 필터링된 값
float lastX = 0.0;  // 이전 추정값
float z = 0.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }

  //print_calibration();
  mpu.verbose(false);
}

float kalmanFilter(float measurement) {
  // 예측 단계
  P = P + Q;

  // 측정 업데이트 단계YIJIY
  K = P / (P + R);
  X = lastX + K * (measurement - lastX);
  P = (1 - K) * P;

  lastX = X;
  Serial.print("Kalman: ");
  Serial.println(X);
  return X;
}

float Read_Height() {
  mpu.update();
  float Height = mpu.getAccZ();
  //Serial.print("Z: ");
  //Serial.println(Height);
  return Height;
}

float Setting_Up() {
  Serial.println("Setting Up Start");

  //float First_Height = Read_Height();
  float First_Height = kalmanFilter(Read_Height());
  float High_Height = -100;

  while (High_Height <= First_Height) {
    High_Height = First_Height;
    Serial.print("High_Height: ");
    Serial.println(High_Height);
    delay(500);
    //First_Height = Read_Height();
    First_Height = kalmanFilter(Read_Height());
  }
  Serial.println("Setting Up Finish");
  return abs(High_Height);
}

float Setting_Down_Ver2() {
  Serial.println("Setting Down Start");

  float First_Height = Read_Height();
  //float First_Height = kalmanFilter(Read_Height());
  float fuck = kalmanFilter(First_Height);
  float Low_Height = 100;
  int Low_flag = 0;

  while (1) {
    while (Low_Height >= First_Height) {
      Low_flag = 0;
      Low_Height = First_Height;
      Serial.print("Low_Height: ");
      Serial.println(Low_Height);
      delay(200);
      //First_Height = Read_Height();
      First_Height = kalmanFilter(Read_Height());
    }
    if (Low_flag >= 3) {
      break;
    }
    Low_flag += 1;
    delay(300);
    //First_Height = Read_Height();
    First_Height = kalmanFilter(Read_Height());
  }
  Serial.println("Setting Down Finish");
  return abs(Low_Height);
}

int During_Exercise(float High_Height, float Low_Height) {
  float First_Height = kalmanFilter(Read_Height());
  delay(100);
  float Seconed_Height = kalmanFilter(Read_Height());
  float High = 0;
  float Low = 0;
  int count = 0;
  int Up_or_Down_flag = 0;
  int During_flag = 0;

  if (First_Height > Seconed_Height) {
    Up_or_Down_flag = 1;
  } 
  else if (First_Height < Seconed_Height) {
    Up_or_Down_flag = 2;
  }

  while (count < 5) {
    if (Up_or_Down_flag == 1) {
      High = Setting_Up();
      delay(100);
      Low = Setting_Down_Ver2();
      During_flag = 1;
    } 
    else if (Up_or_Down_flag == 2) {
      Low = Setting_Down_Ver2();
      delay(100);
      High = Setting_Up();
      During_flag = 1;
    }
    if (abs(High - Low) != 0 && abs(High - Low) >= abs(High_Height - Low_Height) && During_flag == 1) {
      count += 1;
      High = 0; Low = 0;
      During_flag = 0;
      Serial.print("During Count: ");
      Serial.println(count);
    }
  }
  return count;
}

int mmm_flag = 0;

void loop() {!
  mpu.update();

  if (mmm_flag == 0) {
    Serial.println("Start");
    //delay(5000);
    float Height_Hei = Setting_Up();
    
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
