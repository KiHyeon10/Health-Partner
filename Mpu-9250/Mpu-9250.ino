#include "MPU9250.h"

MPU9250 mpu;

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

// 칼만 필터 변수
float P = 1.0; // 추정 오차 공분산
float Q = 0.125; // 프로세스 노이즈 공분산
float R = 4.0; // 관측 노이즈 공분산
float K; // 칼만 이득
float X = 0.0; // 필터링된 값
float lastX; // 이전 추정값

void kalmanFilter(float measurement) {
    // 예측 단계
    P = P + Q;

    // 측정 업데이트 단계
    K = P / (P + R);
    X = lastX + K * (measurement - lastX);
    P = (1 - K) * P;

    lastX = X;
}

void loop() {
  mpu.update();
  print_rawdata();
}

void print_rawdata() {
    Serial.print(mpu.getAccX());
    Serial.print(", ");
    Serial.print(mpu.getAccY());
    Serial.print(", ");
    Serial.print(mpu.getAccZ());
    Serial.print(", ");
    Serial.println();
    delay(500);
}