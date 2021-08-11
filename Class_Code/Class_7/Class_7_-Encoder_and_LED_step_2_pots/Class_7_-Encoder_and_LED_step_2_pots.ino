/*
  Using an encoder to select which LEDs to turn on

  Encoders look like potentiometers but are a collection of switches
  Rather than an analog Voltage to read they send different pulses
  when they are turned left or right or are pressed
  They have much less resolution, a 12b analogRead is 0-4095
  while an encoder will only give 96 values changers per revolution
  Sometimes you want a device that can turn 360 degrees and has no
  absolute position, and has a clear reading though

  Here's a digram of how they work https://www.pjrc.com/teensy/td_libs_Encoder.html

*/

//Interrupts cause everything to stop so one function can happen
// This isn't necessary for encoders and will cause problems with audio
#define ENCODER_DO_NOT_USE_INTERRUPTS

//This library should already be installed. If it's not get it here https://github.com/PaulStoffregen/Encoder/archive/refs/heads/master.zip
#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder
// you can change the name "encoder1" to anything
Encoder encoder1(11, 10);


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

unsigned long current_time;
unsigned long prev_time[8];

int encoder1_position, encoder1_prev_position;
int led_position;
int selected_led;

float hue_bank[19];
float sat_bank[19];
float bright_bank[19];

float hue_pot, sat_pot, bright_pot;

void setup() {
  //nothing to setup for encoders

  LEDs.begin(); //must be done in setup for the addressable LEDs to work.
  //here is a basic way of writing to the LEDs.
  LEDs.setPixelColor(0, 0, 0, 0); //(LED number, red level, green level, blue level). All levels are 0-255
  LEDs.setPixelColor(1, 0, 0, 0);
  LEDs.show(); //send these values to the LEDs

  analogReadResolution(12);  //0-4095
  analogReadAveraging(64);

}



void loop() {
  current_time = millis();

  if (current_time - prev_time[1] > 2) { //33 milliseconds is about 30 Hz, aka 30 fps
    prev_time[1] = current_time;
    //Hey it's that thing we do all the time. Remember then update
    encoder1_prev_position = encoder1_position;
    encoder1_position = encoder1.read();
    if (encoder1_position != encoder1_prev_position) {
      Serial.print(led_position);
      Serial.print(" ");
      Serial.println(encoder1_position);

    }

    //there are only 20 LEDs so lets have another number based on
    //encoder1_position that's kept between 0 and 19
    led_position = (encoder1_position / 2);
    //led_position = constrain(led_position, 0, 19); //another way to do it

    if (led_position < 0) {
      led_position = 0;
    }

    if (led_position > 19) {
      led_position = 19;
    }




  }

  if (current_time - prev_time[0] > 33) { //33 milliseconds is about 30 Hz, aka 30 fps
    prev_time[0] = current_time;

    hue_pot = analogRead(A0) / 4095.0;
    sat_pot = analogRead(A1) / 4095.0;
    bright_pot = analogRead(A2) / 4095.0;

    Serial.println(hue_pot);

    for (byte m = 0; m < 20; m++) {
      set_LED(m, 0, 0, 0); //turn all lights off
    }

    set_LED(led_position, hue_pot, sat_pot, bright_pot);

    LEDs.show(); //send these values to the LEDs
  }

} //loop over



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
