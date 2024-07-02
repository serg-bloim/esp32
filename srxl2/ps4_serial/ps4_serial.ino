// ver 4
#include <PS4Controller.h>
#include <CRC16.h>
#include <CRC.h>

const int LED_PIN = 2;
CRC16 crc(CRC16_XMODEM_POLYNOME,
          CRC16_XMODEM_INITIAL,
          CRC16_XMODEM_XOR_OUT,
          CRC16_XMODEM_REV_IN,
          CRC16_XMODEM_REV_OUT);

union IntToUInt {
  int8_t intVal;
  uint8_t uintVal;
};
union UInt16ToCharArray {
  uint16_t dat;
  char charArray[2];
};
void setup() {
  
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  PS4.begin("a8:2b:b9:45:ac:89");
}

void loop() {
  // Below has all accessible outputs from the controller
  if (PS4.isConnected()) {
    Serial.printf("%4d %4d %4d %4d\n", PS4.LStickX(), PS4.LStickY(), PS4.RStickX(), PS4.RStickY());

    reportPS4State();
    // This delay is to make the output more human readable
    // Remove it when you're not trying to see the output
    delay(0);
  }
  check_led();
}

void check_led() {
  int period = 300;
  int state = (millis()/period) % 2;
  digitalWrite(LED_PIN, state);
}

template<class T, size_t N>
constexpr size_t arraySize(T (&)[N]) {
  return N;
}

void reportPS4State() {
  int8_t params[] = {
    PS4.data.analog.stick.lx,
    -PS4.data.analog.stick.ly,
    PS4.data.analog.stick.rx,
    -PS4.data.analog.stick.ry,
  };
  Serial.printf("%4d %4d %4d %4d\n", params[0], params[1], params[2], params[3]);
  uint8_t len = arraySize(params) + 4 + 2;
  crc.restart();
  writeByte(0xA6);
  //       break;
  //   }
  //   dst.write(byte);
  // } else {
  //   if (serial_state == SERIAL_STATE_WAITING_4_A6 && tx) {
  //     // if (micros() - msg_ended > 1000)
  //         tx = false;
  //         dst.enableTx(tx);
  //   }
  // }


  writeByte(0x10);
  writeByte(len);
  writeByte(0xB5);
  for (auto p : params) {
    IntToUInt data;
    data.intVal = p;
    writeByte(data.uintVal);
  }
  UInt16ToCharArray buf;
  buf.dat = crc.calc();
  Serial1.write(buf.charArray[1]);
  Serial1.write(buf.charArray[0]);
}

void writeByte(uint8_t byte) {
  Serial1.write(byte);
  crc.add(byte);
}