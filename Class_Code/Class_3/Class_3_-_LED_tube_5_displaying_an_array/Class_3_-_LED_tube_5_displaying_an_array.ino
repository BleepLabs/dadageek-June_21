/*
  displaying a random array and use the button to step through modes
*/

//A special library must be used to communicate with the ws2812 addressable LED tube
// Download it here https://github.com/PaulStoffregen/WS2812Serial/archive/refs/heads/master.zip
// Then in Arduino go to "Sketch > Include libraries > Add .ZIP library..." and select the downloaded file.
// The standard LED libraries, like fastLED and adafruits neopixel, cause problems with the audio code we'll do later so this version is used

float max_brightness = .1; //change this to increase the max brightness of the LEDs. 1.0 is very bright

//Everything else can be left alone
#define num_of_leds 20 //number of LEDs in the tube
#include <WS2812Serial.h> //include the code from this file in our sketch  https://github.com/PaulStoffregen/WS2812Serial/archive/refs/heads/master.zip
#define led_data_pin 8 //only pins 1,5, and 8 can be used. 
byte drawingMemory[num_of_leds * 3];
DMAMEM byte displayMemory[num_of_leds * 12];
WS2812Serial LEDs(num_of_leds, displayMemory, drawingMemory, led_data_pin, WS2812_GRB);

//then you can declare variables
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
int latch[4];
//floats can hold decimal values
float lfo[4] = {1, 1, 1, 1}; //set them all to 1
int noodle; //an lfo with a cool name
int button_pin = 5;
int mode1 = 2;
int button_read;
int prev_button_read;
int array_loc;

float hue_array[20] = {.2, .3, .4, .2, .3, .4, .2, .3, .4, .5, .8, .8, .3, .4, .2, .3, .4, .5, .8, .8};
float bright_array[20];
float random_brightness;

void setup() {
  pinMode(button_pin, INPUT_PULLUP);

  LEDs.begin(); //must be done in setup for the addressable LEDs to work.
  //here is a basic way of writing to the LEDs.
  LEDs.setPixelColor(0, 0, 0, 0); //(LED number, red level, green level, blue level). All levels are 0-255
  LEDs.setPixelColor(1, 0, 0, 0);
  LEDs.show(); //send these values to the LEDs

  //for random not to return the same random sequence every time we give it a random number to start with
  // this should only be done once.
  randomSeed(analogRead(A8));

  //fill the arrays with random values
  for (int m = 0; m < 20; m++) {
    hue_array[m] = random(100) / 100.0;
    bright_array[m] = random(2); //random doesn't include highest number so its 0 or 1
  }

} //setup is over


void loop() {
  current_time = millis();

  prev_button_read = button_read;
  button_read = digitalRead(button_pin);

  if (prev_button_read == 1 && button_read == 0) {
    mode1++; //step through modes
    if (mode1 > 3) {
      mode1 = 0; //go back to 0 once it goes over 3
    }
  }

  if (current_time - prev_time[5] > 90) {
    prev_time[5] = current_time;
    random_brightness = random(100) / 99.0;
  }


  if (current_time - prev_time[4] > 150) {
    prev_time[4] = current_time;

    if (mode1 == 2) {
      noodle--;
    }
    if (mode1 == 3) {
      noodle++;
    }

    if (noodle > 19) {
      noodle = 0;
    }
    if (noodle < 0) {
      noodle = 19;
    }
  }

  if (current_time - prev_time[3] > 40) {
    prev_time[3] = current_time;

    lfo[1] = lfo[1] + 0.020;
    if (lfo[1] > 1.0) {
      lfo[1] -= 1.0;
    }

  }

  if (current_time - prev_time[2] > 1) {
    prev_time[2] = current_time;
    if (latch[0] == 1) {
      lfo[0] *= 1.005;
    }
    if (latch[0] == 0) {
      lfo[0] *= .995;
    }
    if (lfo[0] < .01) {
      lfo[0] = .01;
      latch[0] = 1;
    }

    if (lfo[0] > 1) {
      lfo[0] = 1;
      latch[0] = 0;
    }

  }

  if (current_time - prev_time[1] > 33) { //33 milliseconds is about 30 Hz, aka 30 fps
    prev_time[1] = current_time;

    float bright_pot = analogRead(A2) / 1023.0;

    //there's another function in this sketch bellow the loop which makes it easier to control the LEDs more info bellow the loop
    //set_LED(led to change, hue,saturation,brightness)

    int pot1 = analogRead(A0);
    int limit1 = map(pot1, 0, 1023, 0, 20);

    if (mode1 == 2 || mode1 == 3) { //if mode is 2 or 3 show the arrays
      for (int m = 0; m < 20; m++) {
        array_loc = m + noodle; //noodle controls scroll direction
        if (array_loc > 19) {
          array_loc -= 20; //don't reset it, loop it back around
        }
        set_LED(m, hue_array[array_loc], 1, bright_array[array_loc]);
      }
    }


    if (mode1 == 1) { //zones
      for (int m = 0; m < 8; m++) {
        float hue_modulation = m / 50.0;
        set_LED(m, hue_modulation, 1, 1);
      }

      for (int j = 8; j < 16; j++) {
        set_LED(j, .5, 1, random_brightness / 2.0);
      }

      for (int d = 16; d < 20; d++) {
        set_LED(d, .7, 1, random_brightness);
      }
    }

    if (mode1 == 0) { //turn everybody off
      for (int m = 0; m < 20; m++) {
        set_LED(m, 0, 0, 0);
      }
    }

    LEDs.show(); //send these values to the LEDs
  }

  if (current_time - prev_time[0] > 50 && 0) { //change to && 0 to not do this code
    prev_time[0] = current_time;

    Serial.println(noodle);

  }

}// loop is over


////////////////LED function

//This function is a little different than you might see in other libraries but it works pretty similar
// instead of 0-255 you see in other libraries this is all 0-1.0
// you can copy this to the bottom of any code as long as the declarations at the top in "led biz" are done

//set_LED(led to change, hue,saturation,value aka brightness)
// hue - 0 is red, then through the ROYGBIV to 1.0 as red again
// saturation - 0 is fully white, 1 is fully colored.
// value - 0 is off, 1 is the value set by max_brightness
// (it's not called brightness since, unlike in photoshop, we're going from black to fully lit up

//based on https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

void set_LED(int pixel, float fh, float fs, float fv) {
  byte RedLight;
  byte GreenLight;
  byte BlueLight;

  if (fs > 1.0) {
    fs = 1.0;
  }

  if (fv > 1.0) {
    fv = 1.0;
  }

  //wrap the hue around but if it's over 100 or under -100 cap it at that
  if (fh > 100) {
    fh = 100;
  }

  if (fh < -100) {
    fh = -100;
  }
  //keep subtracting or adding 1 until it's in the range of 0-1.0

  while (fh > 1.0) {
    fh -= 1.0;
  }


  while (fh < 0) {
    fh += 1.0;
  }

  byte h = fh * 255;
  byte s = fs * 255;
  byte v = fv * max_brightness * 255;

  h = (h * 192) / 256;  // 0..191
  unsigned int i = h / 32;   // We want a value of 0 thru 5
  unsigned int f = (h % 32) * 8;   // 'fractional' part of 'i' 0..248 in jumps

  unsigned int sInv = 255 - s;
  unsigned int fInv = 255 - f;
  byte pv = v * sInv / 256;  // pv will be in range 0 - 255
  byte qv = v * (256 - s * f / 256) / 256;
  byte tv = v * (256 - s * fInv / 256) / 256;

  switch (i) {
    case 0:
      RedLight = v;
      GreenLight = tv;
      BlueLight = pv;
      break;
    case 1:
      RedLight = qv;
      GreenLight = v;
      BlueLight = pv;
      break;
    case 2:
      RedLight = pv;
      GreenLight = v;
      BlueLight = tv;
      break;
    case 3:
      RedLight = pv;
      GreenLight = qv;
      BlueLight = v;
      break;
    case 4:
      RedLight = tv;
      GreenLight = pv;
      BlueLight = v;
      break;
    case 5:
      RedLight = v;
      GreenLight = pv;
      BlueLight = qv;
      break;
  }
  LEDs.setPixelColor(pixel, RedLight, GreenLight, BlueLight);
}


////////////smooth function
//based on https://playground.arduino.cc/Main/DigitalSmooth/

#define filterSamples   17   // filterSamples should  be an odd number, no smaller than 3. Increase for more smoooothness
#define array_num 8 //number of different smooths we can take, one for each pot
int sensSmoothArray[array_num] [filterSamples];   // array for holding raw sensor values for sensor1

int smooth(int array_sel, int input) {
  int j, k, temp, top, bottom;
  long total;
  static int i;
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[array_sel][i] = input;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j = 0; j < filterSamples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[array_sel][j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {    // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j + 1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1);
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
  }

  return total / k;    // divide by number of samples
}
