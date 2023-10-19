#ifndef SensorCalibration_h
#define SensorCalibration_h

#include <Arduino.h>

class SensorCalibration {
public:
  SensorCalibration(int numSensors); // Constructor that accepts the number of sensors
  void begin();
  void calibrate(int numSamples);
  int getCalibratedValue(int sensorIndex);

private:
  int numSensors;
  int* sensorValues;
};

#endif
