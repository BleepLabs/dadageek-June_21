//Blink one LED at a set rate

// The first section is where we initialize variables we'll use in the code
int led1_output; //int is a 16 bit integer
long current_time; //long is a 32 bit integer
long prev_time;

void setup() {
  //in the setup we execute code that only needs to run one time when the device resets
  // Here we tell the pins we're using if they are input or outputs
  // https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/
  pinMode(5, INPUT_PULLUP); //for the button
  pinMode(9, OUTPUT); // LED
  pinMode(10, OUTPUT); // LED
}

void loop() {
  //the loop run continuously over and over as fast as it can

  //millis returns how many milliseconds have passed since the device reset
  // this value is stored in the variable "current_time" for the rest of this loop
  current_time = millis();

  //The "timing if"
  // when this statement is true the code inside the {} is executed
  if (current_time - prev_time > 500) {
    prev_time = current_time; //remember what time it was when this code was executed

    if (led1_output == 0) { //if it's 0 make it 1...
      led1_output = 1;
    }
    else { //...and visa versa
      led1_output = 0;
    }

    //print this value so we can see it in the serial monitor 
    // tools>serial monitor on the magnifying glass icon in the top right
    Serial.println(led1_output); 

    //send either a 1 or 0, 3.3V or 0V out of this pin
    digitalWrite(9,led1_output); //(pin number,value to send)

  } //"timing if" is over

} //loop is over so the code starts executing from the top again
