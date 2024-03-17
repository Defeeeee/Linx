#ifndef PIDv1_h 
#define PIDv1_h

#include <arduino.h>

class PID{
private:
    float Kp;
    float Ki;
    float Kd;
    float ie;
    float laste;
    float maxie;
    float minie;
    float maxd;
    float mind;
    bool clampie;
    bool clampde;
public:
    PID(float _Kp, float _Ki, float _Kd, bool _clampie = true, float _maxie = 1, float _minie = -1, bool _clampde = true, float _maxd = 0.8, float _mind = -0.8);
    float pid(float e, float step);

};

#endif