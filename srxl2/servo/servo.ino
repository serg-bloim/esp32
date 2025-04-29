#include <ESP32Servo.h>
int servoPin = 25;
Servo servo;
void setup() {
  // put your setup code here, to run once:
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);
  servo.attach(servoPin, 1000, 2000)
  pinMode(servoPin, OUTPUT);

}

void loop() {
  String inp = Serial.readString();
  inp.trim();
  int val = inp.toInt();
  servo.write(val);
}
