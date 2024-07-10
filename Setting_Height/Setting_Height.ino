#include "MPU9250.h"

MPU9250 mpu;

void Check_Height();
float Read_Height();

// 칼만 필터 변수
float P = 1.0;    // 추정 오차 공분산
float Q = 0.125;  // 프로세스 노이즈 공분산
float R = 4.0;    // 관측 노이즈 공분산
float K;          // 칼만 이득
float X = 0.0;    // 필터링된 값
float lastX;      // 이전 추정값
float z = 0.0;

void kalmanFilter(float measurement) {
  // 예측 단계
  P = P + Q;

  // 측정 업데이트 단계YIJIY
  K = P / (P + R);
  X = lastX + K * (measurement - lastX);
  P = (1 - K) * P;

  lastX = X;
}

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

void loop() {
  int flag = 0;
  if (flag == 0) {
    Check_Height();
  } else {
    Serial.println("Finish");
    delay(5000);
  }
}

//높이 차를 저장
void Check_Height() {
  Serial.println("Starting Setting!");
  float Low_Hei = 0;
  float High_Hei = 0;
  float Before_Hei = 0;

  int Low_flag = 0;
  int High_flag = 0;

  while (Low_flag == 0 || High_flag == 0) {
    //Measure Low_height(최저 높이 측정)
    while (Low_flag == 0) {
      Before_Hei = abs(Read_Height());

      if (Low_Hei < Before_Hei) {
        Low_flag = 1;
      } else if (Low_Hei >= Before_Hei) {
        Low_Hei = Before_Hei;
      } else {
        break;
      }
      delay(50);
    }

    //Measure High_height(최고 높이 측정)
    while (High_flag == 0) {
      Before_Hei = abs(Read_Height());

      if (High_Hei >= Before_Hei) {
        High_flag = 1;
      } else if (High_Hei <= Before_Hei) {
        High_Hei = Before_Hei;
      } else {
        break;
      }
      delay(500);
    }
  }
  Serial.print("Low/High Height:  ");
  Serial.print(Low_Hei);
  Serial.print(", ");
  Serial.println(High_Hei);
}

float Read_Height() {
  float x, y, z;

  if (mpu.update()) {
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 25) {

      x = mpu.getAccX();
      y = mpu.getAccY();
      z = mpu.getAccZ();
      Serial.print("Accel: ");
      Serial.print(x, 2);
      Serial.print(", ");
      Serial.print(y, 2);
      Serial.print(", ");
      Serial.println(z, 2);
      //print_roll_pitch_yaw();
      prev_ms = millis();
    }
  }
  return y;
}