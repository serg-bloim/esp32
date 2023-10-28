const int PIN_FWD = 12;
const int PIN_BWD = 14;
#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  10000

int PWM1_DutyCycle = 0;

void setup() {
  ledcAttachPin(PIN_FWD, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
}

void loop() {
  while(PWM1_DutyCycle < 255)
  {
    ledcWrite(PWM1_Ch, PWM1_DutyCycle++);
    delay(10);
  }
  while(PWM1_DutyCycle > 0)
  {
    ledcWrite(PWM1_Ch, PWM1_DutyCycle--);
    delay(10);
  }
}
