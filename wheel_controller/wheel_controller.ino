#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
err
void setup(){}
void loop(){}
#else

#include <HardwareSerial.h>
HardwareSerial SerialPort(1);

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDGamepad.h"
USBHIDKeyboard Keyboard;
USBHIDGamepad Gamepad;

const int buttonPin = 0;          // input pin for pushbutton
int previousButtonState = HIGH;   // for checking the state of a pushButton
int counter = 0;                  // button push counter

void setup() {
  // make the pushButton pin an input:
  pinMode(buttonPin, INPUT_PULLUP);
  // initialize control over the keyboard:
  Keyboard.begin();
  Gamepad.begin();
  USB.begin();
  SerialPort.begin(9600, SERIAL_8N1, 37, 39);
  SerialPort.println("");
  SerialPort.println("Started");
  led_start();
  rotor_init();
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
    Keyboard.println(msg);
  }
  // save the current button state for comparison next time:
  previousButtonState = buttonState;

}
#endif /* ARDUINO_USB_MODE */