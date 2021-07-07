//Adding brigntness control to the previous prompt

// The first section is where we initialize variables we'll use in the code
int led1_output; //int is a 16 bit integer
long current_time; //long is a 32 bit integer
long prev_time;

int pot1; //new variables for the potentiometers
int pot2;
float brightness1;

void setup() {
  //in the setup we execute code that only needs to run one time when the device resets
  // Here we tell the pins we're using if they are input or outputs
  // https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/
  pinMode(5, INPUT_PULLUP); //for the button
  pinMode(9, OUTPUT); // LED
  pinMode(10, OUTPUT); // LED

  //pot pins don't need to be setup
}

void loop() {
  //the loop run continuously over and over as fast as it can

  //millis returns how many milliseconds have passed since the device reset
  // this value is stored in the variable "current_time" for the rest of this loop
  current_time = millis();

  //we want to do this outside of the timing if
  // otherwise the rate will only change when the if is true. Here it will change as the pot changes
  pot1 = analogRead(A0); //returns 0-1023

  //pot reads are 0-1023 but analogwrites are 0-255
  //You could jsut divide by 4 but tahts not obvious but by looking at these funky numbers
  //I like to divide by 1023 then multiply by the number I'd like to output
  pot2 = analogRead(A1);
  brightness1 = (pot2 / 1023.0) * 255.0; //you mush have the ".0" to makse sure it does the match correctly

  //another way is to use map
  //https://www.arduino.cc/reference/en/language/functions/math/map/
  //map(input,original low, original high, new low, new high)
  // 1-1023 is used since the pots dont usually get all the way to 0
  //brightness1 = map(pot3, 1, 1023, 0, 255);

  //The "timing if"
  // when this statement is true the code inside the {} is executed

  //pot 1 goes from 0-1023. We can multiply it by 2 so we can have a slower blink
  if (current_time - prev_time > pot1 * 2) {
    prev_time = current_time; //remember what time it was when this code was executed

    if (led1_output == 0) { //if it's 0 make it 1...
      led1_output = 1;
    }
    else { //...and visa versa
      led1_output = 0;
    }

    //print this value so we can see it in the serial monitor
    // tools>serial monitor on the magnifying glass icon in the top right
    Serial.println(led1_output * brightness1);

    analogWrite(9, led1_output * brightness1); //brightness goes fro 0-255 so when led1_output is 1 it will output that brightness

  } //"timing if" is over

} //loop is over so the code starts executing from the top again
