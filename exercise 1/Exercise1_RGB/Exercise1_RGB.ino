#include <Arduino_LSM6DSOX.h>
#include <WiFiNINA.h>

void setup() {  
  Serial.begin(9600);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // all LED colours off
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() {
  fade(LEDB, LEDG); // blue -> green
  fade(LEDG, LEDR); // green -> red
  fade(LEDR, LEDG); // red -> green
  fade(LEDG, LEDB); // green -> blue
}

void fade(NinaPin from, NinaPin to) {
  for (int rval = 0; rval < 256; rval += 1) {
    // analogWrite: inverted -> 255 = off, 0 = on 
    analogWrite(to, 255-rval);
    analogWrite(from, rval);
    delay(2); // 2 * 255 = 500ms = 1/2 sek
  }
}
