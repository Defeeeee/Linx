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

Ultrasonico :: Ultrasonico(int _trigPinR, int _echoPinR, int _trigPinF, int _echoPinF){
    trigPinR = _trigPinR;
    echoPinR = _echoPinR;
    // trigPinL = _trigPinL;
    // echoPinL = _echoPinL;
    trigPinF = _trigPinF;
    echoPinF = _echoPinF;
    pinMode(trigPinR, OUTPUT);
    pinMode(echoPinR, INPUT);
    // pinMode(trigPinL, OUTPUT);
    // pinMode(echoPinL, INPUT);
    pinMode(trigPinF, OUTPUT);
    pinMode(echoPinF, INPUT);
}

float Ultrasonico :: distanciaR(){
    digitalWrite(trigPinR, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPinR, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinR, LOW);
    float distancia = pulseIn(echoPinR, HIGH);
    distancia = distancia/58;
    return distancia;
}

// float Ultrasonico :: distanciaL(){
//     digitalWrite(trigPinL, LOW);
//     delayMicroseconds(2);
//     digitalWrite(trigPinL, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(trigPinL, LOW);
//     float distancia = pulseIn(echoPinL, HIGH);
//     distancia = distancia/58;
//     return distancia;
// }

float Ultrasonico :: distanciaF(){
    digitalWrite(trigPinF, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPinF, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinF, LOW);
    float distancia = pulseIn(echoPinF, HIGH);
    distancia = distancia/58;
    return distancia;
}

bool Ultrasonico :: checkR(int distance){
    if (distanciaR() < distance) {
        return true;
    } else {
        return false;
    };
}
// bool Ultrasonico :: checkL(int distance){
//     if (distanciaL() > distance) {
//         return true;
//     } else {
//         return false;
//     }
// }

bool Ultrasonico :: checkF(int distance){
    if (distanciaF() < distance) {
        return true;
    } else {
        return false;
    }
}

