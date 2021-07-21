/*
  Adding trails to the LED controlled by the photocell
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
float lfo[4] = {1, 1, 1, 1}; //set them all to 1
float pot[4];
int touch;
int pcell;
int smoothed_pcell;
int smoothed_touch;
int pcell_pos;
int pcell_follower;
float follower1 = 1.0;
float follower1_brightness;

int trail_counter;
int prev_trail_counter;

int trail_len = 19; //set how many trail LEDs we want. The array can hold up to 30
int trails[30];

void setup() {

  LEDs.begin(); //must be done in setup for the addressable LEDs to work.
  //here is a basic way of writing to the LEDs.
  LEDs.setPixelColor(0, 0, 0, 0); //(LED number, red level, green level, blue level). All levels are 0-255
  LEDs.setPixelColor(1, 0, 0, 0);
  LEDs.show(); //send these values to the LEDs

  analogReadResolution(12); //analogRead now returns 0-4095
  analogReadAveraging(64); //each analogread will be averaged before returning a value. This is the first step to smooth readings

} //setup is over


void loop() {
  current_time = millis();

  if (current_time - prev_time[2] > 5) {
    prev_time[2] = current_time;

    pot[0] = analogRead(A0) / 4095.0; //4095 is now the highest reading
    pot[1] = analogRead(A1) / 4095.0;
    pot[2] = analogRead(A2) / 4095.0;


    touch = touchRead(0); // the range of this one is dependent on lots of things so you'll need to print it to see
    smoothed_touch = smooth(1, touch);

    if (follower1 < smoothed_touch) {
      follower1 = follower1 * 1.1;
    }
    if (follower1 >= smoothed_touch) {
      follower1 = follower1 * .995;
    }
    if (follower1 < 1) {
      follower1 = 1;
    }
    follower1_brightness = map(follower1, 1500, 7000, 0, 100);
    follower1_brightness = follower1_brightness / 100.0;

  }


  if (current_time - prev_time[1] > 33) { //33 milliseconds is about 30 Hz, aka 30 fps
    prev_time[1] = current_time;

    for (int m = 0; m < 20; m++) {
      set_LED(m, 0, 0, 0); //turn all lights off
    }

    //to add trails we need to remember where the "pcell_follower" was over time
    // to do this we'll store the readings in an array
    // each time this timing if happens we increment "trail_counter"
    // and store the last reading in that position
    trail_counter++;
    if (trail_counter > trail_len) {
      //when the end of the array is reached we go back to the beginning an overwrite those values
      trail_counter = 0;
    }
    //Then we store the last value in the array
    trails[trail_counter] = pcell_follower;

    //And get a new reading from the pcell
    pcell = analogRead(A3);
    //and map it to the range we want
    pcell_pos = map(pcell, 400, 2000, 0, 19);

    //this follower only allows the LED to move one space at a time
    if (pcell_follower < pcell_pos) {
      pcell_follower++;
    }
    if (pcell_follower > pcell_pos) {
      pcell_follower--;
    }



    //Then we set the previous positions
    // Starting at 0 and going up to how many trailing LEDs we want
    for (int j = 0; j < trail_len; j++) {
      //all the previous positions are stored in the array
      //so we subtract to get to them
      prev_trail_counter = trail_counter - 1 - j;
      //but we don't want to go under 0 so we wrap it around
      //we add the length back to the counter to get to the position we want
      if (prev_trail_counter < 0) {
        prev_trail_counter += trail_len;
      }

      //we can use "j" to change the color and brightness of each trail led

      //.6 and 60 are arbitrary. Try other values to change the rainbow
      float color1 = .6 + (j / 60.0);
      //by diving j by the length we can make the last LED be pretty much all the way faded out
      // trail len is not a float so we make it one so the division works
      float bright1 = 1.0 - (j / float(trail_len));
      set_LED(trails[prev_trail_counter], color1, 1, bright1);

    }

    //Then we set the current position so it will be on top of the trails
    // Otherwise the trails will over write it
    set_LED(pcell_follower, 0, 0, 1);


    LEDs.show(); //send these values to the LEDs
  }

  if (current_time - prev_time[0] > 50 && 0) { //change to && 0 to not do this code
    prev_time[0] = current_time;

    //to have multiple lines in the serial plotter have a " " between the values
    // and println "ln" at the end

    // Serial.print(touch); //print without a return after
    //  Serial.print(" "); //print a space
    Serial.print(pcell_pos);
    Serial.print(" ");
    Serial.println(pcell_follower); //print with a return after

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

byte RGB_calcs[3];

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

//set a certain range of LEDs to an RGB interpolated gradient
// converting to HCL would be an improivemnet but its some heavy duty math https://stackoverflow.com/questions/7530627/hcl-color-to-rgb-and-backward
// but since we're using so little of the LED's range (if briughenss is turned down) it might not make a huge difference
void set_gradient(byte start_LED, byte end_LED, float start_h, float start_s, float start_v, float end_h, float end_s, float end_v) {
  float sRGB[3], eRGB[3], grad[3][20];
  float len = end_LED - start_LED;
  int rev;
  int sL, eL;
  sL = start_LED;
  eL = end_LED;
  if (len < 0) {
    rev = 1;
    len = abs(len);
    sL = end_LED;
    eL = start_LED;
  }

  calc_RGB(start_h, start_s, start_v);
  // Serial.print(sL);  Serial.print(" ");

  for (byte i = 0; i < 3; i++) {
    sRGB[i] = RGB_calcs[i];
    //  Serial.print(sRGB[i]);    Serial.print(" ");
  }
  calc_RGB(end_h, end_s, end_v);
  for (byte i = 0; i < 3; i++) {
    eRGB[i] = RGB_calcs[i];
  }

  LEDs.setPixelColor(sL, sRGB[0], sRGB[1], sRGB[2] );
  LEDs.setPixelColor(eL, eRGB[0], eRGB[1], eRGB[2]);

  //Serial.println(" ");
  float inc = 1;
  for (byte loc = sL + 1; loc < eL; loc++) {
    // Serial.print(loc);    Serial.print(" ");
    for (byte color = 0; color < 3; color++) {
      grad[color][loc] = (((eRGB[color] - sRGB[color]) / len) * inc) + sRGB[color];
      //Serial.print(grad[color][loc]);      Serial.print(" ");
    }
    // Serial.println(" ");
    inc++;
    LEDs.setPixelColor(loc, grad[0][loc], grad[1][loc], grad[2][loc]);
  }
  /*
    Serial.print(eL);  Serial.print(" ");

    for (byte i = 0; i < 3; i++) {
      Serial.print(eRGB[i]);    Serial.print(" ");
    }
    Serial.println();  Serial.println();
  */
}

//convert RGB to and HSV array, don't send to LEDs. used for the gradient
void calc_RGB(float fh, float fs, float fv) {
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
  RGB_calcs[0] = RedLight;
  RGB_calcs[1] = GreenLight;
  RGB_calcs[2] = BlueLight;
}


////////////smooth function
//based on https://playground.arduino.cc/Main/DigitalSmooth/

#define filterSamples   21   // filterSamples should  be an odd number, no smaller than 3. Increase for more smoooothness
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
