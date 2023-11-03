#include "SensorCalibration.h"

#define PINMOTOR1A 8
#define PINMOTOR1B 9
#define PINMOTOR2A 10
#define PINMOTOR2B 11

#define PINMOTORPWM1 13
#define PINMOTORPWM2 12

SensorCalibration::SensorCalibration(int numSensors, int pulseSpd, int Spd) {
  this->numSensors = numSensors;
  sensorValues = new long[numSensors];
  this->pulseSpd = pulseSpd;
  this->Spd = Spd;
}


void SensorCalibration::calibrate(int numSamples, int analogPins[]) {
  digitalWrite(PINMOTOR1A, LOW);
  digitalWrite(PINMOTOR1B, HIGH);
  digitalWrite(PINMOTOR2A, LOW);
  digitalWrite(PINMOTOR2B, HIGH);

  
  analogWrite(PINMOTORPWM1, pulseSpd);
  analogWrite(PINMOTORPWM2, pulseSpd);
  delay(350);
  analogWrite(PINMOTORPWM1, Spd);
  analogWrite(PINMOTORPWM2, Spd);

  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = 0;
    for (int j = 0; j < numSamples; j++) {
      sensorValues[i] = sensorValues[i] + analogRead(analogPins[i]);
    }
    sensorValues[i] = sensorValues[i] / numSamples;
    Serial.println();
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorValues[i]);
  }

  analogWrite(PINMOTORPWM1, 0);
  analogWrite(PINMOTORPWM2, 0);

  digitalWrite(PINMOTOR1A, LOW);
  digitalWrite(PINMOTOR1B, LOW);
  digitalWrite(PINMOTOR2A, LOW);
  digitalWrite(PINMOTOR2B, LOW);
}

float SensorCalibration::getCalibratedValue(int sensorIndex, int analogPins[]) {
  return (float)analogRead(analogPins[sensorIndex]) / (float)(int)sensorValues[sensorIndex];
}