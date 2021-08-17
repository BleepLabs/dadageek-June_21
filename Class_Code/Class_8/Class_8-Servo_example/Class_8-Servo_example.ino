/*
  Using two servos
  A0 pot controls absolute position of servo1 attached to pin 3
  A1 controls position of servo2 on pin 4 but that servo's position is also modulated by an LFO
  A2 controls LFO rate
  LEDs show position of servo2
  
*/

#include <Servo.h>

Servo servo1;  // create servo object to control a servo
Servo servo2;

int pot1_reading;
int pot2_reading;
int pot3_reading;
int servo1_setting;
int servo2_setting;

int lfo1 = 1.0;
int lfo1_top_value = 40;
int lfo1_latch;
int led_lfo1;

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

void setup()
{
  LEDs.begin();

  //Servos can be used with any PWM pins
  servo1.attach(3);  // attaches "servo1" to pin 3
  servo2.attach(4);

  analogReadResolution(12); //0-4095
  analogReadAveraging(64); //take lots of readings and average them each time we do analogRead. This is one stage of smoothing

}

void loop()
{

  current_time = millis();

  pot3_reading = map(analogRead(A2), 0, 4095, 0, 50);

  if (current_time - prev_time[3] > pot3_reading) {
    prev_time[3] = current_time;
    if (lfo1_latch == 1) {
      lfo1++;
    }
    if (lfo1_latch == 0) {
      lfo1--;
    }
    if (lfo1 < 0) {
      lfo1 = 0;
      lfo1_latch = 1;
    }
    if (lfo1 > lfo1_top_value) {
      lfo1 = lfo1_top_value;
      lfo1_latch = 0;
    }
  }
  //10 milliseconds is a good rate to communicate with the servo
  //if you go faster they might move too quickly and burn out 
  if (current_time - prev_time[2] > 10) { 
    prev_time[2] = current_time;
    pot1_reading = analogRead(A0);
    servo1_setting = map(pot1_reading, 0, 4095, 0, 180);  // scale it to use it with the servo, 0 to 180
    servo1.write(servo1_setting); //send the value to the servo

    pot2_reading = analogRead(A1);
    pot2_reading = map(pot2_reading, 0, 4095, 0, 180);
    servo2_setting = constrain(pot2_reading + lfo1, 0, 180); //don't let their combined value it go out of this range

    servo2.write(servo2_setting);
  }

  if (current_time - prev_time[1] > 33) { //33 is 30Hz, about the normal refresh rate of screens
    prev_time[1] = current_time;

    led_lfo1 = map(servo2_setting, 0, 180, 0, 19); //get servo2_setting to the range of the led tube.

    for (byte led_select = 0; led_select < led_lfo1; led_select++) {
      set_LED(led_select, .4, 1, 1);
    }
    for (byte led_select = led_lfo1; led_select < 19; led_select++) {
      set_LED(led_select, 0, 0, 0);
    }
    LEDs.show();

  }


  if (current_time - prev_time[0] > 100) {
    prev_time[0] = current_time;
    Serial.print(lfo1);
    Serial.print(" ");
    Serial.println(pot2_reading);

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
