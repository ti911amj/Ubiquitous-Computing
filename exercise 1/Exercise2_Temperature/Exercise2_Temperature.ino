#include <WiFiNINA.h>
#include <Arduino_LSM6DSOX.h>

int old_val; // 0: temp < 20, 1: 20 <= temp <= 32; 2: temp > 32

void set_led(int);

void setup() {
  // setup LED-Pins
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // all Colours off
  digitalWrite(LEDR, LOW); //RED
  digitalWrite(LEDG, LOW); //GREEN
  digitalWrite(LEDB, LOW); //BLUE

  // setup IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  old_val = -99;
}

void loop() {
  if (IMU.temperatureAvailable())
  {
    int temperature_deg = 0;
    IMU.readTemperature(temperature_deg);
    if (temperature_deg != old_val) {
      set_led(temperature_deg);
    }

    Serial.print("Measured Temperature = ");
    Serial.print(temperature_deg);
    Serial.println(" °C");

    old_val = temperature_deg;
    delay(2000);
  }
}

void set_led(int t) {
  if (t < 20) {
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, HIGH); //BLUE
    digitalWrite(LEDR, LOW);
  } else if (t < 33) {
    digitalWrite(LEDR, LOW); 
    digitalWrite(LEDG, HIGH); //GREEN
    digitalWrite(LEDB, LOW); 
  } else {
    digitalWrite(LEDR, HIGH); //RED
    digitalWrite(LEDG, LOW); //GREEN
    digitalWrite(LEDB, LOW); //BLUE
  }
}