/*
  Potentiometers can be used to put out a variable voltage.
  This voltage is really just data that the Teensy can read using an analog to digital converter pin
  More on this on page 6 of the notes: https://github.com/BleepLabs/dadageek-June_21/raw/main/Extras/Class%201%20-%20Using%20the%20breadboard.pdf

  Here we use the pots to change the rate of blinking in two different ways
*/

float led1_output = 1;
float led2_output = 1;
long current_time;
long prev_time;
long prev_time2;
long prev_print_time;
int button_read;
int prev_button_read;
int latch1 = 0;
int lfo_latch;
int pot1;
int pot2;
float expo_control;

int button_pin = 5;
int led_pin1 = 10;
int led_pin2 = 9;

void setup() {
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led_pin2, OUTPUT);
  pinMode(led_pin1, OUTPUT);
}

void loop() {
  current_time = millis();

  //we don't need to read the pots screaming fast so we slow them down a little with a timing if
  // THis is not a big deal now but later on when our code gets more complicated it will allow things to run more smoothly
  // We can only really perceive the change of the pot this quickly so no big need to do it any faster
  if (current_time - prev_print_time > 10) {
    prev_print_time = current_time;
    pot1 = analogRead(A1); //returns a value fro 0 to 1023 , 10 bits of precision
    pot2 = analogRead(A0);
    Serial.println(expo_control);
  }

  //same button latching as before
  prev_button_read = button_read;
  button_read = digitalRead(button_pin);

  if (prev_button_read == 1 && button_read == 0) {
    //Serial.println("Howdy!");
    if (latch1 == 0) {
      latch1 = 1;
    }
    else {
      latch1 = 0;
    }
  }

  if (latch1 == 0) {

    if (current_time - prev_time2 > 10) {
      prev_time2 = current_time;

      //exponential growth

      //use a pot to change the value we multiply by to grow and shrink led2_output
      // the rate the timing if happens at stays the same but if we lowed it it would also slow down the LFO
      //1023 is the max so div by that to get 0-1.0
      expo_control = (pot2 / 1023.0);

      if (lfo_latch == 1) {
        led2_output *= 1.0 + expo_control; //rising with a number >1
      }

      if (lfo_latch == 0) {
        led2_output *= 1.0 - expo_control; //falling with a number <1
      }

      if (led2_output > 255) { //if we hit the top, keep it there and flip the latch
        led2_output = 255;
        lfo_latch = 0;
      }
      if (led2_output < 1) { //if we hit the bottom keep it from going under 1 and flip the latch
        led2_output = 1;
        lfo_latch = 1;
      }


      analogWrite(led_pin2, led2_output);

    } //end of led2 timing if

    //The rate of this timing if is controlled by pot 2
    if (current_time - prev_time > pot1) {
      prev_time = current_time;

      if (led1_output == 0) {
        led1_output = 1;
      }
      else {
        led1_output = 0;
      }

      analogWrite(led_pin1, led1_output * 64);

    } //end of led1 timing if

  } //end of button if

  else {
    //turn both LEDs off
    analogWrite(led_pin1, 0);
    analogWrite(led_pin2, 0);
  }

} //end of loop
