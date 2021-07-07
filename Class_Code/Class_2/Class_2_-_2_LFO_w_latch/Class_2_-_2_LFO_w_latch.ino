/*
  LFO stand for low frequency oscillator. In synthesis terms it means using an oscillator,
  something that changes periodically, that's going slowly, say oscillation under 100 times a second,
  to modulate aka change a variable.

  Here I've made one that change the brightness of an LED

*/

float led1_output = 1; //we need to use float as we'll be calculating something that has decimals
float led2_output = 1;
long current_time;
long prev_time;
long prev_time2;
int button_read;
int prev_button_read;
int latch1 = 0;
int lfo_latch;

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

  //as we saw in the previous code, the light stay on when the latch is 0
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

      /* Linear LFO that only goes up
        Linear change in lights is less natural looking because of the weird way out senses work
       
        led2_output++; //led1_output=led1_output+1

        if (led2_output > 255) {
        led2_output = 0; //go back to 0 when you hit the top
        }
        
      */

      //exponential rise and fall using a latch

      if (lfo_latch == 1) { //rising when 1
        led2_output *= 1.01; //make the value bigger by multiplying by a number greater than 1.0
      }

      if (lfo_latch == 0) { //falling when 0
        led2_output *= .99; //make the value smaller by multiplying by a number less than 1.0
      }

      if (led2_output > 255) { //if we go over 255, the highest value we can output..
        led2_output = 255; //constrain the value so it can't go over 255
        lfo_latch = 0; //flip the latch so next time around it's falling
      }
      if (led2_output < 1) { //if we're at the bottom...
        led2_output = 1; //keep it at 1 since we cant rise if we multiply by 0
        lfo_latch = 1; //flip the latch so it falls next loop
      }

      Serial.println(led2_output); //print the value in the serial monitor
      //analog write works just like digital write but the output value can be 0-255 levels of brightness, not just on or off
      //  this can only output integers so everything between whole numbers is ignored but we need those decimals to calculate the rise and fall
      analogWrite(led_pin2, led2_output);
      //digitalWrite(led_pin1, led2_output);

    } //end of led2 timing if

    if (current_time - prev_time > 500) {
      prev_time = current_time;

      if (led1_output == 0) {
        led1_output = 1;
      }
      else {
        led1_output = 0;
      }

      analogWrite(led_pin1, led1_output * 10); //output can be 0 to 255. If we multiply led1_output by 10 we get 0 or 10

    } //end of led1 timing if

  } //end of latch if

  else {
    //turn both LEDs off
    analogWrite(led_pin1, 0); //we can't mix digital and analog writes to the same pin so this turns them off
    analogWrite(led_pin2, 0);
  }

} //end of loop
