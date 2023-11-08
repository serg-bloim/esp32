#ifndef MOTOR_H
#define MOTOR_H
#include <inttypes.h>
#include "esp32-hal-ledc.h"

class PWM_OUT{
  private:
    uint8_t pin;
    uint8_t ch;
    int freq;
    uint8_t res;
    int max_val;
  public:
    // PWM_OUT();
    void begin(uint8_t pin, uint8_t pwm_ch, int freq, uint8_t pwm_res);
    void setFract(float val);
};

class Motor{
  private:
  public:
    PWM_OUT pwm_fwd;
    PWM_OUT pwm_bwd;
    // Motor();
    void begin(uint8_t pin_fwd, uint8_t pin_bwd, uint8_t pwm_ch_fwd, uint8_t pwm_ch_bwd, int freq, uint8_t pwm_res);
    void turn(float speed);
    void stop();
};

#endif