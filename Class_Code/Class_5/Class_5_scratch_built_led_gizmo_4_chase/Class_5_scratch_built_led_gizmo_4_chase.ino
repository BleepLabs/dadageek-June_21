/*

Making a new LED thing from scratch
First step
mode 0 
  - all lights flash at different random colors
  - LFO controls brightness
  - LFO shape set by fscale
Mode 1
  - An LED chases across the tube, followed by others
  

pot on A0 selects mode
Pot A1 controls LFO rate
Pot A2 controls LFO shape


*/

float max_brightness = .1; //change this to increase the max brightness of the LEDs. 1.0 is very bright

//Everything else can be left alone
#define num_of_leds 20 //number of LEDs in the tube
#include <WS2812Serial.h> //include the code from this file in our sketch  https://github.com/PaulStoffregen/WS2812Serial/archive/refs/heads/master.zip
#define led_data_pin 8 //only pins 1,5, and 8 can be used. 
byte drawingMemory[num_of_leds * 3];
DMAMEM byte displayMemory[num_of_leds * 12];
WS2812Serial LEDs(num_of_leds, displayMemory, drawingMemory, led_data_pin, WS2812_GRB);

unsigned long current_time;
unsigned long prev_time[8];
int mode;
float random_color;
float lfo;
float lfo_shaped;
int lfo_mode;
float lfo_rise_rate;
float lfo_fall_rate;
int raw_pot_1;
int chase1 = 10;
int chase2 = 10;
float shape1;
int chase1_random;
int chase1_rate = 250;
int chase2_rate = 250;
int prev_chase2;
void setup() {
  LEDs.begin();
}


void loop() {
  current_time = millis();

  //mode = (analogRead(A0) / 1023.0) * 2; //0-2 but 2 is only selected at very end of pot
  mode = map(analogRead(A0), 0, 1023, 0, 2);

  raw_pot_1 = analogRead(A1);
  lfo_rise_rate = raw_pot_1 / 1023.0; //0-1.0
  lfo_rise_rate *= 10.0; // 0 - 10.0
  lfo_rise_rate += 1; //1.0-11.0
  lfo_fall_rate = lfo_rise_rate / 3.0;

  shape1 = analogRead(A2) / 1023.0;
  shape1 *= 20;
  shape1 -= 10; //-10 to 10


  if (current_time - prev_time[1] > 33) {//33milliseonds is 30Hz
    prev_time[1] = current_time;

    if (lfo_mode == 1) {
      lfo += lfo_rise_rate;
    }
    if (lfo_mode == 0) {
      lfo -= lfo_fall_rate;
    }
    if (lfo < 0) {
      lfo = 0;
      lfo_mode = 1;

    }
    if (lfo > 99) {
      lfo = 99;
      lfo_mode = 0;
    }

    // fscale is like map but it allows you to shape the response from logarithmic to exponential. Chonky to snappy
    // Shape is -10 for exponential, 0 for linear, 10 for Log
    // fscale(input, from low,from high,to low,to high,shape) function is at bottom of the code
  
    lfo_shaped = fscale(lfo, 0, 100, 0, 100, shape1);

    if (mode == 0) {
      for (int led_select = 0; led_select < 20; led_select++) {
        random_color = random(100) / 99.0;
        //(led to change, hue,saturation,brightness)
        set_LED(led_select, random_color, 1, lfo_shaped / 99.0);
      }

    }

    if (mode == 1) {
      for (int led_select = 0; led_select < 20; led_select++) {
        set_LED(led_select, .5, 0, 0);
      }
      
      //this one is wiggling around
      //  set_LED(chase1, .5, 1, 1);

      // f < 4 sets how many trailing dots to make
      for (int f = 0; f < 4; f ++) {  

        //prev_chase2 = chase2 - f; //Since we'll do this 4 times there will be 4 trails
        prev_chase2 = chase2 - (f * 2); // they will be spaced out by 2
        //prev_chase2 = chase2 - (f * random(3) + 1); //they will randomly change spacing
        float bright1 = 1.0 - (f / 4.0); //lower brightness for each one. if it was / 3 the last one would be 0 so I picked 4 but try other values
        if (prev_chase2 > 0) {           //only light up led positions that are positive

          set_LED(prev_chase2, .4, 1, bright1);
        }
      }

      set_LED(chase2, .75, 1, 1);


    }

    LEDs.show();
  }

  if (current_time - prev_time[3] > chase2_rate) {
    prev_time[3] = current_time;
    chase2++;

    //get a new rate if it hits position 10 and the top
    if (chase2 == 10) { 
      chase2_rate = random(20, 200);
    }

    if (chase2 > 19) {
      chase2 = 0;
      chase2_rate = random(20, 200);
    }
  }

  if (current_time - prev_time[2] > chase1_rate) {
    prev_time[2] = current_time;

    //wiggle the LED around by randomly deciding if it goes up or down the tube
    chase1_random = random(10);
    if (chase1_random > 4) {
      chase1++;
    }
    if (chase1_random <= 4) {
      chase1--;
    }

    if (chase1 < 0 ) {
      chase1 = 0;
    }

    if (chase1 > 19) { //get a new rate if it hits the top
      chase1 = 19;
      chase1_rate = random(20, 200);
    }
  }


  if (current_time - prev_time[0] > 50) {
    prev_time[0] = current_time;

    Serial.println(chase1_random);

  }

}


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

  //https://playground.arduino.cc/Main/Fscale/ I reordered the input values from this example

  // fscale is like map but it allows you to shape the response from logarithmic to exponential. Chonky to snappy
  // Shape is -10.0 for exponential, 0 for linear, 10.0 for Log
  // fscale(input, from low,from high,to low,to high,shape) function is at bottom of the code
 float fscale( float inputValue, float originalMin, float originalMax,
              float newBegin, float newEnd, float curve) {

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  /*
    Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution
    Serial.println();
  */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin) {
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  /*
    Serial.print(OriginalRange, DEC);
    Serial.print("   ");
    Serial.print(NewRange, DEC);
    Serial.print("   ");
    Serial.println(zeroRefCurVal, DEC);
    Serial.println();
  */

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0) {
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}
