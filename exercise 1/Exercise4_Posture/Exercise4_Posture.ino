// Exercise 4 - Posture
// ------------------ includes ------------------
#include <Arduino_LSM6DSOX.h>  // IMU Bibliothek
#include <WiFiNINA.h>
#include <MadgwickAHRS.h>  // Filter

// ------------------ defines and globals ------------------
int temp;
int temp_check;  // -1, 0, 1 for too cold, ok, too hot

Madgwick filter;
int counter;

// ------------------ function definitions ------------------
void check_temperature();
void alert_and_blink_led(float);

// ------------------ setup and loop ------------------
void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  // IMU setup
  if (!IMU.begin()) {
    Serial.println("IMU initialization failed!");
  }

  // RGB LED setup
  pinMode(LEDR, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDG, OUTPUT);
  analogWrite(LEDR, 255);  // Rot
  analogWrite(LEDG, 255);  // Grün
  analogWrite(LEDB, 255);  // Blau

  //
  counter = 500;
  Serial.println("Initialization");
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println("Hz");
  Serial.println();

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println("Hz");
  Serial.println();
}

void loop() {
  check_temperature();

  // check position
  float aix, aiy, aiz;
  float gix, giy, giz;
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, heading;

  // read raw data from IMU
  IMU.readAcceleration(aix, aiy, aiz);
  IMU.readGyroscope(gix, giy, giz);

  // convert from raw data to gravity and degrees/second units
  ax = convertRawAcceleration(aix);
  ay = convertRawAcceleration(aiy);
  az = convertRawAcceleration(aiz);
  gx = convertRawGyro(gix);
  gy = convertRawGyro(giy);
  gz = convertRawGyro(giz);

  // update the filter, which computes orientation
  filter.updateIMU(gx, gy, gz, ax, ay, az);

  // get heading, pitch and roll
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();
  if (pitch < -100 || pitch > -80) {
    alert_and_blink_led(pitch);
  } else if (counter == 0) {
    Serial.print("Orientation: \n");
    Serial.print(heading);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);
    counter = 500;
  }

  counter--;

  delay(2000);
}

// ------------------ function implementation ------------------
void check_temperature() {
  if (IMU.temperatureAvailable()) {
    IMU.readTemperature(temp);
  }
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("\n");

  if (temp > 19 && temp < 26 && temp_check != 0) {  // temperature ok
    Serial.print("Temperatur ok. \n");
    temp_check = 0;
    analogWrite(LEDR, 255);                    // Rot
    analogWrite(LEDB, 255);                    // Blau
  } else if (temp < 20 && temp_check != -1) {  // it's too cold
    Serial.print("Temperatur zu niedrig. \n");
    temp_check = -1;
    analogWrite(LEDR, 255);                   // Rot
    analogWrite(LEDB, 0);                     // Blau
    delay(500);
  } else if (temp > 25 && temp_check != 1) {  // it's too hot
    Serial.print("Temperatur zu hoch. \n");
    temp_check = 1;
    analogWrite(LEDR, 0);    // Rot
    analogWrite(LEDB, 255);  // Blau
    delay(500);
  }
}

float convertRawAcceleration(int aRaw) {
  // for 2 g range
  // -2 g -> raw value -32768
  // +2 g -> raw value  32767
  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // for range of 250 degrees/seconds
  // -250 -> raw value -32768
  // +250 -> raw value  32767

  float g = (gRaw * 250.0) / 32768.0;
  return g;
}

// used to alert on wrong position
void alert_and_blink_led(float p) {
  Serial.print("Orientation wrong!");
  Serial.print(p);
  Serial.print("\n");

  // LED aus
  analogWrite(LEDR, 255);  // Rot
  analogWrite(LEDG, 255);  // Grün
  analogWrite(LEDB, 255);  // Blau

  delay(1000);

  // blink 5 times
  for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) {                // an
      analogWrite(LEDR, 0);          // Rot
      analogWrite(LEDG, 255 - 165);  // Grün
      analogWrite(LEDB, 255);        // Blau
    } else {                         // aus
      analogWrite(LEDR, 255);        // Rot
      analogWrite(LEDG, 255);        // Grün
      analogWrite(LEDB, 255);        // Blau
    }

    delay(250);
  }

  // LED aus
  analogWrite(LEDR, 255);  // Rot
  analogWrite(LEDG, 255);  // Grün
  analogWrite(LEDB, 255);  // Blau

  temp_check = -99;  // reset so that led will glow up again

  delay(1000);
}