#ifndef SensorCalibration_h
#define SensorCalibration_h

#include <Arduino.h>

class SensorCalibration {
public:
  SensorCalibration(int numSensors, int pulseSpd, int Spd); // Constructor that accepts the number of sensors
  void calibrate(int numSamples, int analogPins[]);
  float getCalibratedValue(int sensorIndex, int analogPins[]);

private:
  int pulseSpd;
  int Spd;
  int numSensors;
  long* sensorValues;
};

#endif
