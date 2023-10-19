#include "SensorCalibration.h"

int analogPins[] = { A5, A6, A7, A8, A9, A10, A11, A12, A13, A14 };

SensorCalibration::SensorCalibration(int numSensors) {
  this->numSensors = numSensors;
  sensorValues = new int[numSensors];
}

void SensorCalibration::begin() {
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = 0;
  }
}

void SensorCalibration::calibrate(int numSamples) {
  for (int i = 0; i < numSensors; i++) {
    for (int j = 0; j < numSamples; j++) {
      sensorValues[i] += analogRead(analogPins[i]);
    }
    sensorValues[i] /= numSamples;
  }
}

int SensorCalibration::getCalibratedValue(int sensorIndex) {
  if (sensorIndex >= 0 && sensorIndex < numSensors) {
    return sensorValues[sensorIndex];
  }
  return -1; // Invalid sensor index
}
