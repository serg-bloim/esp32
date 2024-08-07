// ver 4
#include <PS4Controller.h>
#include <CRC16.h>
#include <CRC.h>
#include <type_traits>
template<typename T1, typename T2>
union _Union {
  T1 v;
  T2 uv;
};
template<typename T>
union CharArrayUnion {
  T v;
  char charArray[sizeof(T)];
};
#define UNION_INT(N) _Union<int##N##_t, uint##N##_t>

const int LED_PIN = 2;
CRC16 crc(CRC16_XMODEM_POLYNOME,
          CRC16_XMODEM_INITIAL,
          CRC16_XMODEM_XOR_OUT,
          CRC16_XMODEM_REV_IN,
          CRC16_XMODEM_REV_OUT);


struct ButtonsResp{
  uint32_t data;
  uint8_t len;
};

void setup() {

  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  PS4.begin("a8:2b:b9:45:ac:89");
}
int ps4 = 0;
void loop() {
  // Below has all accessible outputs from the controller
  auto ms = millis();
  int report_period = 1;
  if (PS4.isConnected() && ms/report_period != ps4) {
    ps4 = ms/report_period;
    reportPS4State();
    // This delay is to make the output more human readable
    // Remove it when you're not trying to see the output
  }
  check_led();
}

void check_led() {
  int period = 300;
  int state = (millis() / period) % 2;
  digitalWrite(LED_PIN, state);
}

template<class T, size_t N>
constexpr size_t arraySize(T (&)[N]) {
  return N;
}

void reportPS4State() {
  int8_t axisParams[] = {
    PS4.data.analog.stick.lx,
    -PS4.data.analog.stick.ly,
    PS4.data.analog.stick.rx,
    -PS4.data.analog.stick.ry,
  };
  uint8_t triggerParams[] = {
    PS4.L2Value(),
    PS4.R2Value(),
  };
  int16_t gyroParams[] = {
    PS4.GyrX(),
    PS4.GyrY(),
    PS4.GyrZ(),
    PS4.AccX(),
    PS4.AccY(),
    PS4.AccZ(),
  };
  auto btns = encodePS4Buttons();
  char bitsString[33]; // Assuming 32-bit int + 1 for null terminator
  Serial.printf("%4d %4d %4d %4d %s\n", axisParams[0], axisParams[1], axisParams[2], axisParams[3], intToBitsString(btns.data, bitsString));
  uint8_t len = 4 + 4 + 4 + sizeof(axisParams) + sizeof(triggerParams) + sizeof(gyroParams) + 2;
  Serial.println("Message of size: " + String(len));
  crc.restart();
  writeByte(0xA6);
  writeByte(0x10);
  writeByte(len);
  writeByte(0xB5);
  writeByte(btns.len);
  writeByte(arraySize(axisParams));
  writeByte(arraySize(triggerParams));
  writeByte(arraySize(gyroParams));
  write(btns.data);
  for (auto p : axisParams) {
    UNION_INT(8)
    data;
    data.v = p;
    writeByte(data.uv);
  }
  for (auto p : triggerParams) {
    writeByte(p);
  }
  for (auto p : gyroParams) {
    UNION_INT(16)
    data;
    data.v = p;
    write(data.uv);
  }
  CharArrayUnion<uint16_t> buf;
  buf.v = crc.calc();
  Serial1.write(buf.charArray[1]);
  Serial1.write(buf.charArray[0]);
}
ButtonsResp encodePS4Buttons() {
  uint32_t data = 0;
  bool buttons[] = {
    PS4.Cross(),
    PS4.Circle(),
    PS4.Triangle(),
    PS4.Square(),
    PS4.Down(),
    PS4.Right(),
    PS4.Up(),
    PS4.Left(),
    PS4.DownRight(),
    PS4.UpRight(),
    PS4.UpLeft(),
    PS4.DownLeft(),
    PS4.L1(),
    PS4.L2(),
    PS4.L3(),
    PS4.R1(),
    PS4.R2(),
    PS4.R3(),
    PS4.PSButton(),
    PS4.Options(),
    PS4.Share(),
    PS4.Touchpad(),
  };
  for(bool btn : buttons){
    data = (data << 1) | btn;
  }
  ButtonsResp resp;
  resp.data  = data;
  resp.len = arraySize(buttons);
  return resp;
}

void writeByte(uint8_t byte) {
  Serial1.write(byte);
  crc.add(byte);
}
template<typename T>
void write(T dat) {
  CharArrayUnion<T> buf;
  buf.v = dat;
  for (auto c : buf.charArray) {
    writeByte(c);
  }
}
char* intToBitsString(int num, char* bitsString) {
    int numBits = sizeof(num) * 8; // Number of bits in an int
    for (int i = numBits - 1; i >= 0; --i) {
        bitsString[numBits - 1 - i] = ((num & (1 << i)) != 0) ? '1' : '0';
    }
    bitsString[numBits] = '\0'; // Null-terminate the string
    return bitsString;
}
