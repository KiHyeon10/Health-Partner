#include <Wire.h>
#include <MPU9250.h>

MPU9250 mpu;

// 필터링을 위한 변수들
float accZFiltered = 0.0;
float accZPrev = 0.0;
float accZPrevFiltered = 0.0;
float accZPrevPrevFiltered = 0.0;

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

float Read_Height() {
  if (mpu.update()) {
    float accZ = mpu.getAccZ();  // 가속도 Z값 읽어오기
    //Serial.print("Z: ");
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

float sum = 0;
void loop() {
  float Z = Read_Height();
  sum += Z;

  Serial.println(sum);
  delay(300);
}