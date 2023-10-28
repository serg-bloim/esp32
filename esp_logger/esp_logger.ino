#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
HardwareSerial SerialPort(2);

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32 Serial Logger"); //Bluetooth device name
  SerialPort.begin (9600, SERIAL_8N1, 16, 17);
  Serial.println("The device started, now you can pair it with bluetooth!");
  led_start();
}

void loop() {
  while (SerialPort.available()) {
    auto  dat = SerialPort.read();
    SerialBT.write(dat);
    Serial.write(dat);
  }
  while (Serial.available()) {
    auto  dat = Serial.read();
    SerialPort.write(dat);
  }
  while (SerialBT.available()) {
    auto  dat = SerialBT.read();
    SerialPort.write(dat);
  }
  delay(1);
}