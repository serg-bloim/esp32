#include <driver/uart.h>


unsigned long previousMillis = 0;
const long interval = 1000;  // Delay of 5 seconds (5000 milliseconds)
bool ledState = false;
const int ledPin = LED_BUILTIN;
#define BAUD 115200
#define RXPIN 4         // GPIO 4 => RX for Serial1
#define TXPIN 5         // GPIO 5 => TX for Serial1

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(TXPIN, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(BAUD, SERIAL_8N1, RXPIN, TXPIN); // Rx = 4, Tx = 5 will work for ESP32, S2, S3 and C3
  Serial1.setHwFlowCtrlMode(HW_FLOWCTRL_DISABLE);
  Serial1.setMode(MODE_RS485_HALF_DUPLEX);
  Serial1.setTimeout(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  flipLED();
  transferSerial();
}

void transferSerial(){
  auto& from = Serial;
  auto& to = Serial1;
  if(from.available()){
    byte buffer[1024];
    int readN = from.readBytes(buffer, 1024);
    // uart_set_rts(UART_NUM_2, 1);
    // uart_set_dtr(UART_NUM_2, 1);
    to.write(buffer, readN);
    to.flush();
    digitalWrite(TXPIN, LOW);
    // uart_set_rts(UART_NUM_2, 0);
    // uart_set_dtr(UART_NUM_2, 0);
    from.write(buffer, readN);
  }
}

void flipLED(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState?HIGH:LOW);
  }
}