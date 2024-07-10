#include <Arduino_LSM9DS1.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");
  if (!IMU.begin()) { // IMU센서를 초기화합니다. 초기화중 문제가 발생하면 오류를 발생시킵니다.
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}
float x, y, z; // 각 센서별로 XYZ값을 저장할 변수입니다.

void loop() {
  delay(500);
  if (IMU.accelerationAvailable()) { // 가속도 센서의 값을 출력합니다.
    IMU.readAcceleration(x, y, z); // x, y, z에 각 축별 데이터를 넣습니다.
    Serial.print("가속도 센서 - ");
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.print(z);
    Serial.println(" G's");
  }

  if (IMU.gyroscopeAvailable()) { // 자이로 센서의 값을 출력합니다.
    IMU.readGyroscope(x, y, z);
    Serial.print("자이로 센서 - ");
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.print(z);
    Serial.println(" degrees/second");
  }
  
  if (IMU.magneticFieldAvailable()) { // 지자기 센서의 값을 출력합니다.
    IMU.readMagneticField(x, y, z);
    Serial.print("지자기 센서 - ");
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.print(z);
    Serial.println(" uT");
  }
  Serial.println();
  Serial.println();
}