/*
  Using the Bleep labs MMA8451 accelerometer
  the board we're using is similar to this https://www.adafruit.com/product/2019 but I made it a long time ago before it was available from adafruit but never put it in a product
*/

//these must be included for the accelo to work.
// Find them in sketch>include library > manage libraries and search for "MMA8451" and "Adafruit unified Sensor" which will be at the bottom of the list
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

//wire aka i2c communication is already on your computer
#include <Wire.h>

//name the snesor and get the library working
Adafruit_MMA8451 accelo = Adafruit_MMA8451();

unsigned long current_time, prev[4];
int x_read, y_read, z_read;
int orientation;
int  smoothed_x;
int  smoothed_y;
int  smoothed_z;

void setup(void) {
  delay(100);


  accelo.begin(0x1C); //the adafruit board uses a different address

  //can be "MMA8451_RANGE_2_G", "MMA8451_RANGE_4_G" or "MMA8451_RANGE_8_G"
  // the bigger the number,the less sensitive
  accelo.setRange(MMA8451_RANGE_2_G);

}

void loop() {
  current_time = millis();

  // Read the raw data. fills the x y z with bipolar 14 bit values, -8191 to 8191
  accelo.read();

  //we don't need all that resolution so lets divide it down to -1000 to 1000 to make it easier to understand
  // since 8191 is our biggest number just move the decimal in that and divide by it
  x_read = accelo.x / 8.191; //these can only be done after accelo.read()
  y_read = accelo.y / 8.191;
  z_read = accelo.z / 8.191;



  if (current_time - prev[0] > 100) {
    prev[0] = current_time;



    Serial.print(x_read);
    Serial.print(" ");
    Serial.print(y_read);
    Serial.print(" ");
    Serial.print(z_read);
    Serial.println();


    //this device works just like the accelo in your phone that rotates the screen.
    // this function gets a value from 0-8 which then we print as a phone based orientation.
    orientation = accelo.getOrientation();

    //"switch case" is similar to "if" but you give it a pile of possible outcomes and it picks just one
    // then it "breaks", leaving the switch and going on to the next thing
    // https://www.arduino.cc/reference/en/language/structure/control-structure/switchcase/
    switch (orientation) {
      //these are defines in the adafruit library. "#define MMA8451_PL_PUF 0" https://github.com/adafruit/Adafruit_MMA8451_Library/blob/c7f64f04f00a16b6c786677db4fc75eec65fabdd/Adafruit_MMA8451.h#L45
      // so this is just the same as saying:
      // case 0:
      case MMA8451_PL_PUF:
        Serial.println("Portrait Up Front");
        //you could put whatever you want here. maybe it could trigger different sounds
        break;
      case MMA8451_PL_PUB:
        Serial.println("Portrait Up Back");
        break;
      case MMA8451_PL_PDF:
        Serial.println("Portrait Down Front");
        break;
      case MMA8451_PL_PDB:
        Serial.println("Portrait Down Back");
        break;
      case MMA8451_PL_LRF:
        Serial.println("Landscape Right Front");
        break;
      case MMA8451_PL_LRB:
        Serial.println("Landscape Right Back");
        break;
      case MMA8451_PL_LLF:
        Serial.println("Landscape Left Front");
        break;
      case MMA8451_PL_LLB:
        Serial.println("Landscape Left Back");
        break;
    }
    Serial.println(); // print a return to space it out

    //keep starting it so you can plug the wires in as it's running,
    // allowing you to fix any wiring issues and see if it works immediatley
    // usually you only want to do this in setup
    accelo.begin(0x1C);

  }
} // end of loop
