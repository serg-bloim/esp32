#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
err void setup() {}
void loop() {}
#else
#define M_STRING(s) #s
#define M_WRAP(s) M_STRING(s)
#define INSPECT(v) Serial.println(String(#v) + " : " + M_WRAP(v))
#include <HardwareSerial.h>
#include "motor.h"
// ConfigManager cfgManager(SerialPort);
#include "USB.h"
#include "USBHIDMouse.h"
// #include "USBHIDGamepad.h"
#include <Joystick_ESP32S2.h>
// USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;
// USBHIDGamepad Gamepad;
const uint8_t MY_JOYSTICK_REPORT_ID = 0x05;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_MULTI_AXIS, 0, 0,
                   false, false, false, false, false, false,
                   false, false,
                   false, false, true);

Motor motor;
const int btn_look_left = 3;
const int btn_look_right = 5;
const int buttonPin = 0;         // input pin for pushbutton
int previousButtonState = HIGH;  // for checking the state of a pushButton
int counter = 0;                 // button push counter
const float rotation_speed = 0.5;
const int wheel_center_position = 512;
void setup() {
  // make the pushButton pin an input:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(btn_look_left, INPUT_PULLUP);
  pinMode(btn_look_right, INPUT_PULLUP);
  // initialize control over the keyboard:
  // Keyboard.begin();
  // Gamepad.begin();
  Mouse.begin();
  USB.begin();
  #define CORE_DEBUG_LEVEL 2
  Serial.setPins(1, 2);
  Serial.begin(9600);
  Serial.println("Start Serial");
  
  INSPECT(ARDUHAL_LOG_LEVEL_ERROR);
  INSPECT(CORE_DEBUG_LEVEL);
  INSPECT(ARDUHAL_LOG_LEVEL);
  INSPECT(USE_ESP_IDF_LOG);
  INSPECT(LOG_LOCAL_LEVEL);
  INSPECT(log_e);
  INSPECT(ESP_LOGE);
  INSPECT(LOG_TEST);
  INSPECT(COMPILER_OPT);
  
  ESP_LOGE("123", "%d", 987);
  log_e("1234567890");
  log_printf("1111111111111");
  led_start();
  rotor_init();
  Joystick.begin();
  Joystick.setAccelerator(0);
  motor.begin(18, 33, 0, 1, 10000, 8);
}
int head_dir = 0;
int head_state = 0;
int head_step = 10;
int head_boundary = 600;
int time_since_last_loop = 0;
unsigned long time_last_loop = 0;
void loop() {
  // read the pushbutton:
  int buttonState = digitalRead(buttonPin);
  // if the button state has changed,
  if ((buttonState != previousButtonState)
      // and it's currently pressed:
      && (buttonState == LOW)) {
    // increment the button counter
    counter++;
    // type out a message
    auto msg = "You pressed the button " + String(counter) + " times.";
    Serial.println(msg);
    // Keyboard.println(msg);
  }
  // save the current button state for comparison next time:
  previousButtonState = buttonState;
  int rotor_pos = rotor_read();
  Joystick.setSteering(wheel_center_position + (rotor_pos * rotation_speed));
  if(!motor_boundary_react(rotor_pos, 1200, -1) &&
     !motor_boundary_react(rotor_pos, -1200, 1)){
    motor.stop();
  }
  if(digitalRead(btn_look_left) == LOW){
    if( head_dir != -1){
    head_dir = -1;
    Mouse.move(0, 0);
    }
    // Serial.println("Turning left");
  }else if(digitalRead(btn_look_right) == LOW){
    if(head_dir != 1){
    head_dir = 1;
    Mouse.move(0, 0);
    }
    // Serial.println("Turning right");
  }else{
    if(head_dir != 0){
    head_dir = 0;
    // Mouse.move(1, 0);
    }
  }
  turn_head();
}
void turn_head(){
  int goal = head_dir*head_boundary;
  int step = goal - head_state;
  step = constrain(step, -head_step, head_step);
  auto now = millis();
  if(step != 0 && now-time_last_loop >= 0){
    if(Mouse.hid.ready()){
      time_last_loop = now;
      head_state += step;
      Mouse.move(step, 0);
      // Serial.println(String("step: ")+step + " head_state: " + head_state);
      // delay(10);
    }else{
      Serial.println("Mouse hid is not ready");
    }
  }
}
bool motor_boundary_react(int rotor_pos, int limit, int dir) {
  String log = String("motor_boundary_react(")+rotor_pos + ", " + limit + ", "+ dir+ ")";
  if (limit < 0) {
    limit = -limit;
    rotor_pos = -rotor_pos;
  }
  if (rotor_pos > limit) {
    int diff = rotor_pos - limit;
    const int max_in = 100;
    const int max_out = 100;
    const float factor = 0.2;
    int reaction = constrain(map(diff, 0, max_in, 0, max_out), 0, max_out);
    motor.turn(factor * dir * reaction / max_out);
    Serial.println(log + " = " + (dir * reaction));
    return true;
  }
  return false;
}
#endif /* ARDUINO_USB_MODE */