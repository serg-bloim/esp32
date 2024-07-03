#include "SoftwareSerial.h"

const int uart_data_pin = 4;
const int LED_PIN = 2;
EspSoftwareSerial::UART swSer1;
void setup() {
  pinMode(LED_PIN, OUTPUT);
  delay(2000);
  Serial.begin(115200);
  // Serial.println("GpioCapabilities:: hasPullUp(rxPin) = " + String(GpioCapabilities:: hasPullUp(uart_data_pin)));
  Serial.setRxTimeout(2);
  swSer1.enableRxGPIOPullUp(true);
  swSer1.begin(115200, EspSoftwareSerial::SWSERIAL_8N1, uart_data_pin, uart_data_pin, false, 256);
  // high speed half duplex, turn off interrupts during tx
  swSer1.enableIntTx(false);
  swSer1.enableTx(true);
  delay(123);
  swSer1.enableTx(false);
}


void loop() {
  check_serial();
  check_led();
}

void check_led() {
  int period = 1000;
  int state = (millis()/period) % 2;
  digitalWrite(LED_PIN, state);
}

const int SERIAL_STATE_WAITING_4_A6 = 1;
const int SERIAL_STATE_WAITING_4_LEN = 2;
const int SERIAL_STATE_READING = 3;
int msg_len = 0;
int serial_state = SERIAL_STATE_WAITING_4_A6;
int msg_ended = 0;
bool tx = false;
void check_serial() {
  char hexString[3];  // Buffer to hold the hexadecimal string (2 characters + null terminator)
  auto& src = Serial;
  auto& dst = swSer1;
  if (!tx && dst.available())
    Serial.write(dst.read());
  if (src.available()) {
    auto byte = src.read();
    sprintf(hexString, "%02X", byte);
    switch (serial_state) {
      case SERIAL_STATE_WAITING_4_A6:
        if (byte == 0xA6) {
          serial_state = SERIAL_STATE_WAITING_4_LEN;
          msg_len = 1;
          tx = true;
          dst.enableTx(tx);
        } else {
          // Serial.println("Unexpected char while waiting for 0xA6: " + String(hexString));
        }
        break;
      case SERIAL_STATE_WAITING_4_LEN:
        if (msg_len > 0) msg_len--;
        else {
          msg_len = byte - 3;
          serial_state = SERIAL_STATE_READING;
          // Serial.println("Sending message of len: " + String(byte));
          break;
        }
      case SERIAL_STATE_READING:
        // dst.write(byte);
        msg_len--;
        if (msg_len == 0) {
          serial_state = SERIAL_STATE_WAITING_4_A6;
          msg_ended = micros();
        }
        break;
    }
    dst.write(byte);
  } else {
    if (serial_state == SERIAL_STATE_WAITING_4_A6 && tx) {
          tx = false;
          dst.enableTx(tx);
    }
  }
}