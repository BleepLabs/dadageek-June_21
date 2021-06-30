//Randonly cange the rate one of the LEDs blinks at

int led1_output;
int led2_output;
long current_time;
long prev_time;
long prev_time2;
int button_read;
int interval1 = 200; //we can initialize a variable with any value we want
int interval2;

void setup() {
  pinMode(5, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  current_time = millis();

  button_read = digitalRead(5); //0 when button is pressed

  if (button_read == 1) {

    if (current_time - prev_time2 > interval1) {
      prev_time2 = current_time;

      //https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
      //random returns a number "randomly" picked between the high and low values every time it is executed
      // here it's run inside the timing if so a new interval is picked each time the previous one has been used
      // if this is moved outside the timing if, it will be making random numbers 100ks of times a second so the LED output will be very different
      interval1 = random(100, 1000); //returns something from 100 to 999 (the top number is not included for some reason)

      if (led2_output == 0) {
        led2_output = 1;
      }
      else {
        led2_output = 0;
      }

      digitalWrite(10, led2_output);

    } //end of led2 timing if

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

    } //end of led1 timing if

  } //end of button if

  else {
    digitalWrite(9, 0);
    digitalWrite(10, 0);
  }

} //end of loop
