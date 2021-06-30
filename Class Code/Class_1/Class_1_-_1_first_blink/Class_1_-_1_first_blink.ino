
int led1_output;
long current_time;
long prev_time;

void setup() {
  pinMode(5, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  current_time = millis();

  if (current_time - prev_time > 500) {
    prev_time = current_time;

    if (led1_output == 0) {
      led1_output = 1;
    }
    else {
      led1_output = 0;
    }
    Serial.println(led1_output);

    digitalWrite(9,led1_output);

  }

}
