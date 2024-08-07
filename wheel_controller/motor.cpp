#include "Arduino.h"
#include "HardwareSerial.h"
#include <cmath>
#include <cstdlib>
#include "motor.h"

void Motor::begin(uint8_t pin_fwd, uint8_t pin_bwd, uint8_t pwm_ch_fwd, uint8_t pwm_ch_bwd, int freq, uint8_t pwm_res){
  pwm_fwd.begin(pin_fwd, pwm_ch_fwd, freq, pwm_res);
  pwm_bwd.begin(pin_bwd, pwm_ch_bwd, freq, pwm_res);
}

void Motor::turn(float speed){
    PWM_OUT& move_a = pwm_fwd;
    PWM_OUT& move_b = pwm_bwd;
    if (speed == 0){
      this->stop();
    }
    if (speed > 0){
      pwm_fwd.setFract(speed);
      pwm_bwd.setFract(0);
    }else{
      pwm_bwd.setFract(-speed);
      pwm_fwd.setFract(0);
    }
}

void Motor::stop(){
    pwm_fwd.setFract(0);
    pwm_bwd.setFract(0);
}

void PWM_OUT::begin(uint8_t pin, uint8_t pwm_ch, int freq, uint8_t pwm_res){
  this->pin = pin;
  this->ch = pwm_ch;
  this->freq = freq;
  this->res = pwm_res;
  this->max_val = 1<<res;
  ledcSetup(pwm_ch, freq, res);
  ledcAttachPin(pin, pwm_ch);
}

void PWM_OUT::setFract(float val){
  auto duty = 0;
  if (val > 0){
    duty = constrain(max_val * val, 0, max_val);
  }
  ledcWrite(ch, duty);
}