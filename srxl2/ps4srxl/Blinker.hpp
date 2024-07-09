#pragma once
#include <Arduino.h>

class Blinker
{
private:
  size_t pin;
  size_t delay;
  size_t next_ts = millis() + delay;
  bool state = false;

public:
  Blinker(size_t pin, size_t delay) : pin(pin), delay(delay) {}
  void begin(){
    pinMode(pin, OUTPUT);
  }
  void update(){
    // Serial.println(1);
    if(millis() > next_ts){
      state = !state;
      digitalWrite(pin, state);
      next_ts += delay;
    }
  }
};