int led1_output;
int led2_output;
long current_time;
long prev_time;
long prev_time2;

void setup() {
  pinMode(5, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  current_time = millis();

  if (current_time - prev_time2 > 100) {
    prev_time2 = current_time;

    if (led2_output == 0) {
      led2_output = 1;
    }
    else {
      led2_output = 0;
    }    

    digitalWrite(10, led2_output);

  }

  if (current_time - prev_time > 500) {
    prev_time = current_time;

    if (led1_output == 0) {
      led1_output = 1;
    }
    else {
      led1_output = 0;
    }    
    Serial.println(led1_output);    
    
    digitalWrite(9, led1_output);

  }

}
