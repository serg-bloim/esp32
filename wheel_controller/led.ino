
void led_task(void * parameter){
  const int LED_PIN = 15;
  pinMode(LED_PIN, OUTPUT);
  bool state = false;
  Serial.println("LED task is running");
  while (true) {
    digitalWrite(LED_PIN, state);
    state = !state;
    vTaskDelay(500);
  }
}
void led_start(){
  Serial.println("Creating the LED task");
  xTaskCreate(
    led_task,
    "TSK_LED",
    1000,
    NULL,
    1,
    NULL
  );
  Serial.println("Created the LED task");
}