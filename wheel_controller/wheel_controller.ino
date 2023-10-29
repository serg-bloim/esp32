#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
err
void setup(){}
void loop(){}
#else

#include <HardwareSerial.h>
#include "motor.cpp"
HardwareSerial SerialPort(1);
// ConfigManager cfgManager(SerialPort);
// #include "USB.h"
// #include "USBHIDKeyboard.h"
// #include "USBHIDGamepad.h"
#include <Joystick_ESP32S2.h>
// USBHIDKeyboard Keyboard;
// USBHIDGamepad Gamepad;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, 0, 0,
  false, false, false, false, false, false,
  false, false,
  false, false, true);

Motor motor();

const int buttonPin = 0;          // input pin for pushbutton
int previousButtonState = HIGH;   // for checking the state of a pushButton
int counter = 0;                  // button push counter
void setup() {
  // make the pushButton pin an input:
  pinMode(buttonPin, INPUT_PULLUP);
  // initialize control over the keyboard:
  // Keyboard.begin();
  // Gamepad.begin();
  USB.begin();
  SerialPort.begin(9600, SERIAL_8N1, 3, 5);
  SerialPort.println("");
  SerialPort.println("Started");
  led_start();
  rotor_init(0, 1024, 2);
  Joystick.begin();
  Joystick.setAccelerator(0);
}
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
    SerialPort.println(msg);
    // Keyboard.println(msg);
  }
  // save the current button state for comparison next time:
  previousButtonState = buttonState;
  Joystick.setSteering(rotor_read());
  delay(10);
}
#endif /* ARDUINO_USB_MODE */