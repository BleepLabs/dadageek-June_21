//Blink two LEDs at separate rates when a button is pressed

//To have a second LED blinking at a rate different than the first one
// we need separate variables for everything
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
  //same setup as before
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led_pin2, OUTPUT);
  pinMode(led_pin1, OUTPUT);
}

void loop() {
  current_time = millis();

  if (current_time - prev_print_time > 10) {
    prev_print_time = current_time;
    pot1 = analogRead(A0); //0-1023 input range
    pot2 = analogRead(A1);
    Serial.println(expo_control);
  }

  //Read the pin and see if it's connected to 3.3V or 0V
  // by default it is "pulled high" to 3.3V
  // so its returns 0 when button is pressed
  // and 1 when not pressed

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

  // only do what's inside these {} when the button is pressed
  if (latch1 == 0) {

    //same as before but now it's prev_time2,led2_output, and pin 10
    if (current_time - prev_time2 > 10) {
      prev_time2 = current_time;
      /* Linear LFO
        led2_output++; //led1_output=led1_output+1

        if (led2_output > 255) {
        led2_output = 0;
        }
      */

      //exponetial growth

      expo_control = (pot2 / 1023.0); //1023 is the max so div by that to get 0-1.0

      if (lfo_latch == 1) {
        led2_output *= 1.0 + expo_control;
      }

      if (lfo_latch == 0) {
        led2_output *= 1.0 - expo_control;
      }

      if (led2_output > 255) {
        led2_output = 255;
        lfo_latch = 0;
      }
      if (led2_output < 1) {
        led2_output = 1;
        lfo_latch = 1;
      }


      analogWrite(led_pin2, led2_output);
      //digitalWrite(led_pin1, led2_output);

    } //end of led2 timing if

    if (current_time - prev_time > pot1) {
      prev_time = current_time;

      if (led1_output == 0) {
        led1_output = 1;
      }
      else {
        led1_output = 0;
      }

      analogWrite(led_pin1, led1_output * 64); //output can be 0-255 so 64 is about 1/4 bright

    } //end of led1 timing if

  } //end of button if

  //if the button is not being pressed do this

  else {
    //turn both LEDs off
    analogWrite(led_pin1, 0);
    analogWrite(led_pin2, 0);
  }

} //end of loop
