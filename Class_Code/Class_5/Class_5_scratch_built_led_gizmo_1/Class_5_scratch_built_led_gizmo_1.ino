/*

Making a new LED thing from scratch
First step
mode 0 - all lights flash at different random colors

pot on A0 selects mode

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

void setup() {
  LEDs.begin();
}


void loop() {
  current_time = millis();

  //mode = (analogRead(A0) / 1023.0) * 2; //0-2 but 2 is only selected at very end of pot, when the analog read=1023
  mode = map(analogRead(A0), 0, 1023, 0, 2); //give 0 1 and 2 in equal thirds of the pot

  if (current_time - prev_time[1] > 33) { //33 is 30Hz, about the normal refresh rate of screens
    prev_time[1] = current_time;

    if (mode == 0) {  

      //When this line is here the random color is calculated each time the timing if happens
      // This means all the LEDs will be the same color
      //random is an integer. random(100) gives 0-99 so we divide it by 99.0 to get it to 0-1.0
      //random_color = random(100) / 99.0; 

      for (byte led_select = 0; led_select < 20; led_select++) { 
        //When the line is here, a new random color is calculated each for loop, so once her led
        // making them all different 
        random_color = random(100) / 99.0;

        //(led to change, hue 0-1.0,saturation 0-1.0,brightness 0-1.0)
        set_LED(led_select, random_color, 1, 1);
      }
      LEDs.show();
    }
  }



  if (current_time - prev_time[0] > 100) {
    prev_time[0] = current_time;

    Serial.println(mode);

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
