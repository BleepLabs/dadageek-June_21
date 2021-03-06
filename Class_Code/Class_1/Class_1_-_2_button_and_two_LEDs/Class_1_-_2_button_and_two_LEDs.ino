//Blink two LEDs at separate rates when a button is pressed

//To have a second LED blinking at a rate different than the first one
// we need separate variables for everything
int led1_output;
int led2_output;
long current_time;
long prev_time;
long prev_time2;
int button_read;

void setup() {
  //same setup as before
  pinMode(5, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  current_time = millis();

  //Read the pin and see if it's connected to 3.3V or 0V
  // by default it is "pulled high" to 3.3V
  // so its returns 0 when button is pressed
  // and 1 when not pressed
  button_read = digitalRead(5);

  // only do what's inside these {} when the button is pressed
  if (button_read == 0) {

    //same as before but now it's prev_time2,led2_output, and pin 10
    if (current_time - prev_time2 > 100) {
      prev_time2 = current_time;

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

  //if the button is not being pressed do this
  else {
    //turn both LEDs off
    digitalWrite(9, 0);
    digitalWrite(10, 0);
  }

} //end of loop
