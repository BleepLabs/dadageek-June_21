//The single button will latch the light on or off when pressed
// instead of having them only on while the button is being held down

int led1_output;
int led2_output;
long current_time;
long prev_time;
long prev_time2;
int button_read;
int prev_button_read;
int latch1 = 1; //we can initialize a variable with any value

int button_pin = 5; //making these variables lets us refer to them with words rather than a pin number that we might not remember
int led_pin1 = 10;
int led_pin2 = 9;

void setup() {
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led_pin2, OUTPUT);
  pinMode(led_pin1, OUTPUT);

}

void loop() {
  current_time = millis();

  //Before we update button_read by checking if the pin is high or low
  // we remember what it was last loop. This allow us to see if there was a change
  prev_button_read = button_read;
  button_read = digitalRead(button_pin);

  /*
    The code in this "if" will only happen if the button was not being pressed and then it was aka the falling edge
    Buttons have four states: 
    Not being pressed. digitalRead(button_pin) returns 1
    Being pressed. digitalRead(button_pin) returns 0
    Falling. Was NOT being pressed last loop but IS pressed this loop
    Rising. WAS being pressed last loop but IS NOT pressed this loop
  */
  
  if (prev_button_read == 1 && button_read == 0) { //did the button fall?
    Serial.println("Howdy!"); //print these characters inside the " "

    //Just like flipping the LED back and forth, we can use if and else to flip the latch so it stays at a value
    if (latch1 == 0) {
      latch1 = 1;
    }
    else {
      latch1 = 0;
    }

  }

  if (latch1 == 0) {

    //same as before but now it's prev_time2,led2_output, and pin 10
    if (current_time - prev_time2 > 100) {
      prev_time2 = current_time;

      if (led2_output == 0) {
        led2_output = 1;
      }
      else {
        led2_output = 0;
      }

      digitalWrite(led_pin1, led2_output);

    } //end of led2 timing if

    if (current_time - prev_time > 500) {
      prev_time = current_time;

      if (led1_output == 0) {
        led1_output = 1;
      }
      else {
        led1_output = 0;
      }

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
