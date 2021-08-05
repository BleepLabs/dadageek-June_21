/*
  Playing two drum sound with buttons
  Three pots are used to change their sound
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthSimpleDrum     drum2;          //xy=242.8835296630859,399.17140324910474
AudioSynthWaveform       waveform2;      //xy=243.71686299641922,295.8380699157714
AudioSynthWaveform       waveform1;      //xy=251.2168526649475,211.67138290405273
AudioSynthSimpleDrum     drum1;          //xy=272.05018615722656,354.17138671875
AudioMixer4              mixer1;         //xy=454.5501963297525,255.83806991577146
AudioOutputAnalog        dac1;           //xy=626.2168629964192,235.83806991577146
AudioConnection          patchCord1(drum2, 0, mixer1, 3);
AudioConnection          patchCord2(waveform2, 0, mixer1, 1);
AudioConnection          patchCord3(waveform1, 0, mixer1, 0);
AudioConnection          patchCord4(drum1, 0, mixer1, 2);
AudioConnection          patchCord5(mixer1, dac1);
// GUItool: end automatically generated code


//Bounce is another way of reading button and can remove double triggering
// Download it here https://github.com/thomasfredericks/Bounce2/archive/refs/heads/master.zip
// Install it by going to Sketch > Include library > add zip library
#include <Bounce2.h>

//set the number of buttons and the pins they are connected to
#define NUM_BUTTONS 2 //a define is just a replacement. It can't be changed. You can use it to set the size of an array. A normal variable can't do that
const int BUTTON_PINS[NUM_BUTTONS] = {2, 5};
Bounce * buttons = new Bounce[NUM_BUTTONS];
#define BOUNCE_LOCK_OUT //THis is the better way to use bounce with audio https://github.com/thomasfredericks/Bounce2#lock-out-interva


float max_brightness = .1; //change this to increase the max brightness of the LEDs. 1.0 is very bright

//Everything else can be left alone
#define num_of_leds 20 //number of LEDs in the tube
#include <WS2812Serial.h> //include the code from this file in our sketch  https://github.com/PaulStoffregen/WS2812Serial/archive/refs/heads/master.zip
#define led_data_pin 8 //only pins 1,5, and 8 can be used. 
byte drawingMemory[num_of_leds * 3];
DMAMEM byte displayMemory[num_of_leds * 12];
WS2812Serial LEDs(num_of_leds, displayMemory, drawingMemory, led_data_pin, WS2812_GRB);


// Note frequencies starting at midi note 12, C0. https://newt.phys.unsw.edu.au/jw/notes.html
// If you want to play a midi note then just subtract 12 from it and plug it into the array. A4, 440.0Hz midi note 69, so chromatic[57] for example
// "PROGMEM const static" means we'll store this in memory not RAM. We not be able to chance it but we don't need to
// This array is pretty big and RAM is precious but we've got plenty of flash aka program storage space
PROGMEM const static float chromatic[121] = {16.3516, 17.32391673, 18.35405043, 19.44543906, 20.60172504, 21.82676736, 23.12465449, 24.499718, 25.95654704, 27.50000365, 29.13523896, 30.86771042, 32.7032, 34.64783346, 36.70810085, 38.89087812, 41.20345007, 43.65353471, 46.24930897, 48.99943599, 51.91309407, 55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192, 8869.845359, 9397.273811, 9956.06479, 10548.08321, 11175.30488, 11839.82309, 12543.8556, 13289.75207, 14080.00185, 14917.24233, 15804.26772, 16744.03838};

//then we're back to declaring variables as usual
unsigned long current_time;
unsigned long prev_time[8];
float freq1, freq2;
int note_select1, note_select2;
float amp1, amp2;
float drum1_fade, drum2_fade;
float drum_len;
float drum_pitch_env;

void setup() {

  LEDs.begin(); //must be done in setup for the addressable LEDs to work.
  //here is a basic way of writing to the LEDs.
  LEDs.setPixelColor(0, 0, 0, 0); //(LED number, red level, green level, blue level). All levels are 0-255
  LEDs.setPixelColor(1, 0, 0, 0);
  LEDs.show(); //send these values to the LEDs

  analogReadResolution(12); //0-4095
  analogReadAveraging(64); //take lots of readings and average them each time we do analogRead. This is one stage of smoothing

  //set all the buttons up for bounce. There are only to so the for is maybe unnecessary but it's scalable
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  ); //setup the bounce instance for the current button
    buttons[i].interval(10);  // interval in milliseconds. How long after the first change will ignore noise
  }


  // The audio library uses blocks of a set size (256 bytes) so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // The Teensy 3.2 has enough memory for about 250 blocks but then there's no space for anything else
  // It's usually the delay and reverb that hog it. Most other things only need a block

  AudioMemory(10);
  dac1.analogReference(EXTERNAL); //set the output to be the full 0-3.3V, making it better for use with headphones.


  //Each audio object in the gui has it's on functions.
  // for most you'll need to set them up in setup

  //https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform
  //begin(amplitude 0-1.0, frequency, shape)
  //the amplitude will usually  be 1 and we'll attenuate it later.
  // Frequency is the starting pitch
  // Useful shapes are: WAVEFORM_SINE,WAVEFORM_TRIANGLE, WAVEFORM_BANDLIMIT_SAWTOOTH, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE, WAVEFORM_BANDLIMIT_SQUARE ,WAVEFORM_BANDLIMIT_PULSE
  waveform1.begin(1, 220, WAVEFORM_SINE);
  waveform2.begin(1, chromatic[57], WAVEFORM_SINE); //play 440.0 A4

  //https://www.pjrc.com/teensy/gui/?info=AudioSynthSimpleDrum
  drum1.frequency(80); //starting freq
  drum1.length(100);//how long to fade out in milliseconds
  drum1.pitchMod(.8); //less than .5 the pitch will rise, great and it will drop

  drum2.frequency(440); //starting freq
  drum2.length(500);//how long to fade out in milliseconds
  drum2.pitchMod(.3); //less than .5 the pitch will rise, great and it will drop

  //You can attach a single output to multiple inputs but to combine signals you need a mixer
  // 1.0 is the maximum volume. Over that and you clip. So you need to attenuate signals
  //This doesn't mean the all the gains should add up to 1.0, but it's good to start that way and increase the volume to see what works.
  //gain(input 0-3,amplitude 0-1.0)
  //amplitude can go from -32766.0 to 32767.0 but usually we're just attenuating, not amplifying by 30000. Negative numbers flip the phase
  mixer1.gain(0, .5);
  mixer1.gain(1, .5);
  mixer1.gain(2, .5);
  mixer1.gain(3, .5);

} //setup is over

void loop() {
  current_time = millis();

  for (int j = 0; j < NUM_BUTTONS; j++)  {
    buttons[j].update();  //must be done when using bounce. This remembers the previous reads and updates all the buttons

    if ( buttons[j].fell() ) {
      //do something when any button falls
    }

    if ( buttons[j].rose() ) {
      //do something when any button rises
    }
  }

  if ( buttons[0].fell() ) {
    drum1.noteOn();
    drum1_fade = 1.0; //set the brightness to 1
  }

  if ( buttons[1].fell() ) {
    drum2.noteOn();
    drum2_fade = 1.0; //set the brightness to 1
  }

  if (buttons[0].read() == 0) {
    //do something while the button on the left is held down
    //drum1.noteOn(); // you don't want this here as it will keep restarting the sound every loop
  }


  if (current_time - prev_time[2] > 5) { //slowing  down analog reads a little makes them less noisy
    prev_time[2] = current_time;

    freq1 = analogRead(A0) / 4.0; //0-1000ish
    drum_len = analogRead(A1) / 4.0; //0-1000
    drum_pitch_env = analogRead(A2) / 4095.0; //0-1.0

    //the waveforms are still playing but their mixer inputs are turned all the way down.
    note_select2 = map(analogRead(A1), 0, 4095, 40, 80);
    freq2 = chromatic[note_select2];
    waveform1.frequency(freq1);
    waveform2.frequency(freq2);


    drum1.frequency(freq1); //starting freq
    drum1.length(drum_len);//how long to fade out in milliseconds
    drum1.pitchMod(drum_pitch_env); //less than .5 the pitch will rise, great and it will drop

    //Same values used but multiplied or divided
    drum2.frequency(freq1 * 2); //starting freq
    drum2.length(drum_len / 3); //how long to fade out in milliseconds
    drum2.pitchMod(drum_pitch_env); //less than .5 the pitch will rise, great and it will drop


    //amp1 = analogRead(A2) / 4095.0; //0-1 which will cause clipping
    amp1 = .7; //seems to be as loud as they can go without clipping
    mixer1.gain(0, 0); //waveform1
    mixer1.gain(1, 0); //waveform2
    mixer1.gain(2, amp1); //drum1
    mixer1.gain(3, amp1); //drum2
  }


  if (current_time - prev_time[1] > 33) { //33 milliseconds is about 30Hz
    prev_time[1] = current_time;

    //reduce these values exponentially
    // I picked some values that looked good for the sounds. you could control them with the amplitude of the sound directly with peak https://www.pjrc.com/teensy/gui/?info=AudioAnalyzePeak
    drum1_fade *= .75;
    drum2_fade *= .75;

    //This isn't really necessary but shows how to turn them off when they get bellow a set level
    if (drum1_fade < .05) {
      drum1_fade = 0;
    }
    if (drum2_fade < .05) {
      drum2_fade = 0;
    }

    //LEDs 0 - 9
    for (byte led_select = 0; led_select < 10; led_select++) {
      set_LED(led_select, .25, 1, drum1_fade);
    }

    //LEDs 9-19
    for (byte led_select = 10; led_select < 19; led_select++) {
      set_LED(led_select, .4, 1, drum2_fade);
    }

    LEDs.show();
  }

  if (current_time - prev_time[0] > 500 && 1) {
    prev_time[0] = current_time;

    //Here we print out the usage of the audio library
    // If we go over 90% processor usage or get near the value of memory blocks we set aside in the setup we'll have issues or crash.
    // If you're using too many block, jut increase the number up top until you're over it by a couple
    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("%    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    Serial.println();
    Serial.println(amp1);
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
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
