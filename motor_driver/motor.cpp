#include "HardwareSerial.h"
#include <cmath>
#include <cstdlib>
#include "motor.h"

void Motor::begin(uint8_t pin_fwd, uint8_t pin_bwd, uint8_t pwm_ch_fwd, uint8_t pwm_ch_bwd, int freq, uint8_t pwm_res){
  pwm_fwd.begin(pin_fwd, pwm_ch_fwd, freq, pwm_res);
  pwm_bwd.begin(pin_bwd, pwm_ch_bwd, freq, pwm_res);
}

void Motor::turn(float speed){
    PWM_OUT& move = speed>0 ? pwm_fwd : pwm_bwd;
    move.setFract(std::abs(speed));
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
  ledcAttachPin(pin, pwm_ch);
  ledcSetup(pwm_ch, freq, res);
}

void PWM_OUT::setFract(float val){
    ledcWrite(ch, max_val * val);
}