//How does the timing if really work?
//Here we print all the values used for tining


unsigned long current_time;
unsigned long prev_time[8];
float k;
int rate1;
int bananana;

void setup() {
  randomSeed(analogRead(A9));
}

void loop() {
  current_time = millis();

  rate1 = analogRead(A0); //0-1023

  if (current_time - prev_time[1] > rate1) {
    prev_time[1] = current_time;
    //! mean opposite
    // if something that is 0 or 1 is set to it's opposite it becomes 1 or 0
    // Only works with 0 and 1
    bananana = !bananana;
  }


  if (current_time - prev_time[0] > 5) {
    prev_time[0] = current_time;
    Serial.println(bananana);
    Serial.print(prev_time[1]);
    Serial.print(" ");
    Serial.print(current_time);
    Serial.print(" ");
    Serial.print(current_time - prev_time[1]);
    Serial.println();
  }

}
