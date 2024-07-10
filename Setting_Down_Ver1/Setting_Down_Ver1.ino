void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  
}

float Setting_Down() {
  Serial.println("Setting Down Start");

  //float First_Height = Read_Height();
  float First_Height = kalmanFilter(Read_Height());
  float Low_Height = 100;

  while (Low_Height >= First_Height) {
    Low_Height = First_Height;
    Serial.print("Low_Height: ");
    Serial.println(Low_Height);
    delay(500);
    //First_Height = Read_Height();
    First_Height = kalmanFilter(Read_Height());
  }
  Serial.println("Setting Down Finish");
  return abs(Low_Height);
}