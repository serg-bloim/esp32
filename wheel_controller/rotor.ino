#define PIN_ROTOR_DATA_1 37
#define PIN_ROTOR_DATA_2 39
const int RESET_BTN_PIN = 0;

volatile int encoder_value = 0; // Global variable for storing the encoder position
volatile int changed_times = 0; // Global variable for storing the encoder position
volatile int rtr_min = 0;
volatile int rtr_max = 100;
volatile float rtr_factor = 1;

void rotor_init(){
  pinMode(PIN_ROTOR_DATA_1, INPUT_PULLUP);
  pinMode(PIN_ROTOR_DATA_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ROTOR_DATA_1), rotor_upd, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RESET_BTN_PIN), zero_wheel, RISING);
  // rotor_monitor_start();
  zero_wheel();
}
void zero_wheel(){
  encoder_value = 0;
}
void rotor_upd(){
  // Reading the current state of encoder A and B
  changed_times++;
  int A = digitalRead(PIN_ROTOR_DATA_1);
  int B = digitalRead(PIN_ROTOR_DATA_2);
  int new_val = encoder_value;
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
  Serial.println("Rotor monitor task is running");
  while (true) {
    Serial.println("Rotor state: " + String(rotor_read()));
    vTaskDelay(1000);
  }
}
void rotor_monitor_start(){
  Serial.println("Creating the Rotor monitor task");
  xTaskCreate(
    rotor_monitor_task,
    "TSK_ROT_MON",
    1000,
    NULL,
    1,
    NULL
  );
  Serial.println("Created the Rotor monitor task");
}