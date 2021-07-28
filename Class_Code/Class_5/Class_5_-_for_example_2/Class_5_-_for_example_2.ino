unsigned long current_time;
unsigned long prev_time[8];
float k;
void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(A9));
}

void loop() {
  current_time = millis();

  if (current_time - prev_time[1] > 1000) {
    prev_time[1] = current_time;

    for (int j = 0; j < 10; j++) {
      k = random(100) / 99.0;
      //k = k + 1000;
      Serial.print(j);
      Serial.print("-");
      Serial.print(k, 6);
      Serial.print("  ");
    }
    Serial.println();
  }

  

}
