// #include <Wire.h>
// #include <MPU9250_asukiaaa.h>

// MPU9250_asukiaaa mpu;

//#define G_SCALE_FACTOR 16384.0f  // ±2g 감도 설정에 대한 스케일 팩터

float Z = 0.0;

void setup() {
  Serial.begin(9600);
  //Wire.begin();
  //mpu.setWire(&Wire);
  //mpu.beginAccel();
  //mpu.accelUpdate();
  //Z = mpu.accelZ();
}
int flag_sdfdfg = 0;

void loop() {
  
  Serial.println("Start");
  Serial.println(flag_sdfdfg);
  // static unsigned long prevTime = millis();
  float prevAccZ = Z;  // 이전 가속도 값 저장
  static float velocity = 0.0;
  static float distance = 0.0;
  // // MPU-9250에서 가속도 값을 읽어옴
  // mpu.accelUpdate();

  // // 가속도 값 읽기
  // float accZ = mpu.accelZ() - prevAccZ;  // Z 축 가속도 값만 사용
  // Serial.println("Prepare");
  // // 현재 시간 계산
  // unsigned long currentTime = millis();
  // float deltaTime = (currentTime - prevTime) / 1000.0;  // 시간 간격 (초 단위)

  // 가속도 값이 변하지 않으면 속도와 거리를 업데이트하지 않음
  // if (accZ == prevAccZ) {
  //   // 이전 시간 변수 업데이트
  //   prevTime = currentTime;

  //   // 이전 가속도 값 업데이트
  //   prevAccZ = accZ;

  //   // 결과 출력
  //   Serial.print("Distance: ");
  //   Serial.print(distance, 4);
  //   Serial.println(" meters");

  //   delay(100);  // 적절한 딜레이 추가

  //   return;
  // }
  // else{
  //   continue;
  // }

  float avgAccZ = 0;
  if(flag_sdfdfg == 0){
    //avgAccZ = accZ;
    flag_sdfdfg = 1;
    Serial.print("fuck");
    Serial.println("1");
  }
  else{
    //avgAccZ = (accZ + prevAccZ) / 2.0;
    Serial.println('2');
  }

//   // 속도를 계산 (적분)
//   Serial.println(Z);
//  // Serial.println(accZ);
//   Serial.println(prevAccZ);
//   Serial.println(avgAccZ);
//   delay(1000);
  //velocity += avgAccZ / G_SCALE_FACTOR; * deltaTime;

  // 거리를 계산 (적분)
  //distance += velocity * deltaTime;

  // 이전 시간 및 가속도 변수 업데이트
  //prevTime = currentTime;
  //prevAccZ = accZ;

  // 결과 출력
  //Serial.print("Distance: ");
  //Serial.print(distance, 4);
  //Serial.println(" meters");

  delay(1000);  // 적절한 딜레이 추가
}
