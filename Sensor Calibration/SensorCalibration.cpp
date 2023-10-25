#include "SensorCalibration.h"


SensorCalibration::SensorCalibration(int numSensors) {
  this->numSensors = numSensors;
  sensorValues = new int[numSensors];
}

void SensorCalibration::begin() {
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = 0;
  }
}

void SensorCalibration::calibrate(int numSamples, int analogPins[]) {
  for (int i = 0; i < numSensors; i++) {
    for (int j = 0; j < numSamples; j++) {
      sensorValues[i] += analogRead(analogPins[i]);
      delay(2);
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
