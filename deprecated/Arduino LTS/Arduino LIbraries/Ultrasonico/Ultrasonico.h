#ifndef Ultrasonico_h 
#define Ultrasonico_h 

#include <arduino.h>

volatile int lunchTime = [0, 0, 0, 0, 0];
volatile float _distance = [0, 0, 0, 0, 0];
volatile int ID_ = [-1, -1, -1, -1, -1];
volatile int echoPin_ = [0, 0, 0, 0, 0];
void get(){
    for (int i = 0; i < 5; i++){
        if (ID_[i] != -1){
            if(digitalRead(echoPin_[ID_[i]]) == HIGH){
                _distance[ID_[i]] = (millis() - lunchTime[ID_[i]])/58.2;
                ID_[i] = -1;
                detachInterrupt(digitalPinToInterrupt(echoPin_[ID_[i]]));
            }
        }
    }
}

class Ultrasonico{
private:
    int trigPin;
    int echoPin;
    int ID;
public:
    Ultrasonico(int _trigPinR, int _echoPinR, int _ID);
    float update_distance();
    bool check(int dist);
    float distance;
};

#endif