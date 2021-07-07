/*
  The first two pots control the individual blink rates
  The third pot controls both their brightness levels

*/

float led1_output = 1; //floats can store decimal numbers
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
int pot3;
float max_bright;
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
    pot3 = analogRead(A2);
    //we want max_bright to go from 0-1.0. This will give us a flexible value to work with
    max_bright = (pot3 / 1023.0); //dividing by 1023 makes it 0-1.0
    
    Serial.println(max_bright);
  }

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

    //same as before but now it's prev_time2,led2_output, and pin 10
    if (current_time - prev_time2 > 10) {
      prev_time2 = current_time;

      //exponetial growth
      expo_control = (pot2 / 1023.0); //1023 is the max so div by that to get 0-1.0

      if (lfo_latch == 1) {
        led2_output *= 1.0 + expo_control; //rising
      }

      if (lfo_latch == 0) {
        led2_output *= 1.0 - expo_control; //falling
      }

      if (led2_output > 255) {
        led2_output = 255;
        lfo_latch = 0;
      }
      if (led2_output < 1) {
        led2_output = 1;
        lfo_latch = 1;
      }

      //led2 output goes from 0-255 so multiplying by 0-1.0 will attenuate it
      analogWrite(led_pin2, led2_output * max_bright); 

    } //end of led2 timing if

    if (current_time - prev_time > pot1) {
      prev_time = current_time;

      if (led1_output == 0) {
        led1_output = 1;
      }
      else {
        led1_output = 0;
      }

      //led1_output is jsut 0 or 1. We multiply by 255 to get it to be 0 or 255
      // then we multiply by 0-1.0 to attenuate it
      analogWrite(led_pin1, led1_output * 255 * max_bright);

    } //end of led1 timing if

  } //end of latch if

  else {
    //turn both LEDs off
    // stick to using either analog or digial wite for each LED. Don't mix the functions
    analogWrite(led_pin1, 0);
    analogWrite(led_pin2, 0);
  }

} //end of loop
