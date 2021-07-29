unsigned long current_time;
unsigned long prev_time[8];
float k;
int rate1;
int bananana;
void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(A9));
}

void loop() {
  current_time = millis();

  rate1 = analogRead(A0);

  if (current_time - prev_time[1] > rate1) {
    prev_time[1] = current_time;
    bananana = !bananana; //ONLY FOR 0 AND 1
  }


  if (current_time - prev_time[0] > 5  && 1) {
    prev_time[0] = current_time;
    Serial.println(bananana);

    //    Serial.print(prev_time[1]);
    //    Serial.print(" ");
    //    Serial.print(current_time);
    //    Serial.print(" ");
    //    Serial.print(current_time - prev_time[1]);
    //    Serial.println();
  }

}
