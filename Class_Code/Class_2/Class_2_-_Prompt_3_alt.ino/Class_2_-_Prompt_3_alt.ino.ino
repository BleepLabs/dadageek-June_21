//One pot changes the base rate that one light blinks at
// another changes the ratio the other will blink at. 2x as fast, 3x, 4x... as the base rate
// The third pot controls brightness

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
int base_rate;
int rate2;
float fade_time;
float brightness1;

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
    pot1 = analogRead(A1); //0-1023 input range
    pot2 = analogRead(A0);
    //https://www.arduino.cc/reference/en/language/functions/math/map/
    //map(input,original low, original high, new low, new high)
    base_rate = map(pot1, 1, 1023, 10, 1000);
    rate2 = map(pot2, 1, 1023, 1, 8);

    brightness1 = analogRead(A2) / 1023.0; //0-1.0
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
    //bases rate divided by rate 2 means this if will be locked to the other one but at differnt ratios. If you multipled it would be longer tahtn the other if
    if (current_time - prev_time2 > base_rate / rate2) {
      prev_time2 = current_time;

      if (led2_output == 0) {
        led2_output = 1;
      }
      else {
        led2_output = 0;
      }

      analogWrite(led_pin2, led2_output * 255 * brightness1);  //led2_output and brightness are just 0-1 so we multiply by 22 to get the whole range

    } //end of led2 timing if

    if (current_time - prev_time > base_rate) {
      prev_time = current_time;

      if (led1_output == 0) {
        led1_output = 1;
      }
      else {
        led1_output = 0;
      }

      analogWrite(led_pin1, led1_output * 255 * brightness1); 

    } //end of led1 timing if

  } //end of button if

  //if the button is not being pressed do this

  else {
    //turn both LEDs off
    analogWrite(led_pin1, 0);
    analogWrite(led_pin2, 0);
  }

} //end of loop
