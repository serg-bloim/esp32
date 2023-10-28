#define PIN_ROTOR_DATA_1 37
#define PIN_ROTOR_DATA_2 39

volatile int encoder_value = 0; // Global variable for storing the encoder position
volatile int changed_times = 0; // Global variable for storing the encoder position
volatile int rtr_min = 0;
volatile int rtr_max = 100;
volatile float rtr_factor = 1;

void rotor_init(int min, int max, float factor){
  pinMode(PIN_ROTOR_DATA_1, INPUT_PULLUP);
  pinMode(PIN_ROTOR_DATA_2, INPUT_PULLUP);
  rtr_min = (int)(min*factor);
  rtr_max = (int)(max*factor);
  rtr_factor = factor;
  attachInterrupt(digitalPinToInterrupt(PIN_ROTOR_DATA_1), rotor_upd, CHANGE);
  rotor_monitor_start();
}
void rotor_upd(){
  // Reading the current state of encoder A and B
  changed_times++;
  int A = digitalRead(PIN_ROTOR_DATA_1);
  int B = digitalRead(PIN_ROTOR_DATA_2);
  int new_val = encoder_value;
  // If the state of A changed, it means the encoder has been rotated
  if ((A == HIGH) != (B == LOW)) {
    new_val--;
  } else {
    new_val++;
  }
  encoder_value = cap(new_val, rtr_min, rtr_max);
}

int rotor_read(){
  return (int)(encoder_value/rtr_factor);
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