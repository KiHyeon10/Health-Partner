#include "MPU9250.h"

// MPU9250 객체 생성
MPU9250 mpu;

// 저역통과 필터 상수
const float alpha = 0.8; // 필터 강도

// 이전 측정값 초기화
float prevAccZ = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(2000);
    
    // MPU9250 설정
    if (!mpu.setup(0x68)) { // MPU9250의 I2C 주소인 0x68로 설정
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }

    // 필터링을 위해 이전 측정값 초기화
    prevAccZ = mpu.getAccZ();

    // 센서 출력 설정
    mpu.verbose(false);
}

void loop() {
    // MPU 업데이트
    if (mpu.update()) {
        // 현재 측정값 읽기
        float currentAccZ = mpu.getAccZ();

        // 저역통과 필터 적용
        float filteredAccZ = lowPassFilter(currentAccZ, prevAccZ, alpha);

        // 결과 출력
        Serial.print("Filtered AccZ: ");
        Serial.println(filteredAccZ);
        Serial.print("Z: ");
        Serial.println(currentAccZ);

        // 이전 측정값 업데이트
        prevAccZ = filteredAccZ;

        delay(500);
    }
}

// 저역통과 필터 함수 정의
float lowPassFilter(float newValue, float oldValue, float alpha) {
    return alpha * newValue + (1 - alpha) * oldValue;
}
