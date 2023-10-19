#define PIN_ROTOR_DATA_1 3
#define PIN_ROTOR_DATA_2 5
void rotor_init(){
  pinMode(PIN_ROTOR_DATA_1, INPUT_PULLUP);
  pinMode(PIN_ROTOR_DATA_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ROTOR_DATA_1), rotor_upd, CHANGE);
  rotor_monitor_start();
}

volatile int encoder_value = 0; // Global variable for storing the encoder position
volatile int changed_times = 0; // Global variable for storing the encoder position
void rotor_upd(){
  // Reading the current state of encoder A and B
  changed_times++;
  int A = digitalRead(PIN_ROTOR_DATA_1);
  int B = digitalRead(PIN_ROTOR_DATA_2);
  // If the state of A changed, it means the encoder has been rotated
  if ((A == HIGH) != (B == LOW)) {
    encoder_value--;
  } else {
    encoder_value++;
  }
}

int rotor_read(){
  return encoder_value;
}

void rotor_monitor_task(void * parameter){
  SerialPort.println("Rotor monitor task is running");
  while (true) {
    SerialPort.println("Rotor state: " + String(rotor_read()));
    vTaskDelay(1000);
  }
}
void rotor_monitor_start(){
  SerialPort.println("Creating the Rotor monitor task");
  xTaskCreate(
    rotor_monitor_task,
    "TSK_ROT_MON",
    1000,
    NULL,
    1,
    NULL
  );
  SerialPort.println("Created the Rotor monitor task");
}