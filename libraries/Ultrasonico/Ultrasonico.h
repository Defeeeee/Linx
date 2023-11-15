#ifndef Ultrasonico_h 
#define Ultrasonico_h 

#include <arduino.h>

class Ultrasonico{
private:
    int trigPinR;
    int echoPinR;
    // int trigPinL;
    // int echoPinL;
    int trigPinF;
    int echoPinF;
public:
    Ultrasonico(int _trigPinR, int _echoPinR, int _trigPinF, int _echoPinF);
    float distanciaR();
    // float distanciaL();
    float distanciaF();
    bool checkR(int distance);
    // bool checkL(int distance);
    bool checkF(int distance);
};

#endif