#include <PIDv1.h>

PID :: PID(float _Kp, float _Ki, float _Kd, bool _clampie = true, float _maxie = 1, float _minie = -1, bool _clampde = true, float _maxd = 0.8, float _mind = -0.8){
    Kp = _Kp;
    Ki = _Ki;
    Kd = _Kd;
    clampie = _clampie;
    maxie = _maxie;
    minie = _minie;
    clampde = _clampde;
    maxd = _maxd;
    mind = _mind;
}

float PID :: pid(float e, float step){
    //integration
    ie = ie + e;

    float d = Kd * (e - laste) / step; //derivation
    //clamping
    if (clampie == true && ie > maxie) ie = maxie;
    if (clampie == true && ie < minie) ie = minie;
    if (clampde == true && d > maxd) d = maxd;
    if (clampde == true && d < mind) d = mind;
    //pid
    float output = Kp * e + Ki * ie + d;
    laste = e; //update laste
    return output;
}

