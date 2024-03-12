#include <Ultrasonico.h>

/**
 * @brief Constructor for the Ultrasonico class.
 * 
 * This constructor initializes the Ultrasonico object with the specified trigger and echo pins for the right and front sensors.
 * It sets the pin modes for the trigger and echo pins.
 * 
 * @param _trigPinR The pin number for the right sensor's trigger pin.
 * @param _echoPinR The pin number for the right sensor's echo pin.
 * @param _trigPinF The pin number for the front sensor's trigger pin.
 * @param _echoPinF The pin number for the front sensor's echo pin.
 */

Ultrasonico :: Ultrasonico(int _trigPinR, int _echoPinR, int _ID){
    trigPin = _trigPin;
    echoPin = _echoPin;
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    ID = _ID;
    echoPin_[ID] = echoPin;
}

float Ultrasonico :: update_distance(){
    distance = _distance[ID];
    for(int i = 0; i < 5; i++){
        if(ID_[i] == -1){
            ID_[i] = ID;
            break;
        }
    }
    lunchTime[ID] = millis();
    attachInterrupt(digitalPinToInterrupt(echoPin), get, RISING);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
}

bool Ultrasonico :: check(int dist){
    if (distance < dist) {
        return true;
    } else {
        return false;
    };
}
