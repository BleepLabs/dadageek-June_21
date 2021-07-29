//explaining the for loop

unsigned long current_time;
unsigned long prev_time[8];
int k;
void setup() {

  //random seed reads an unconnected analog pin to get random() started off with a "more random" value.
  // otherwise it returns the same random values each time you restart
  // this only needs to be done once so its in setup
  randomSeed(analogRead(A9));
}

void loop() {
  current_time = millis();


  if (current_time - prev_time[1] > 1000) {
    prev_time[1] = current_time;

    //j is how many steps to take.
    // k happens that many steps
    for (int j = 0; j < 10; j++) {
      k = random(100);
      Serial.print(j);
      Serial.print("-");
      Serial.print(k);
      Serial.print("  ");
    }

    Serial.println(); //put an return AFTER the for so it only happens once

  }
}
