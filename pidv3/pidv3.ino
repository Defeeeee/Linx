#include <SensorCalibration.h>
#include <PIDv1.h>

#define PINMOTOR1A 8
#define PINMOTOR1B 9
#define PINMOTOR2A 10
#define PINMOTOR2B 11

#define PINMOTORPWM1 13
#define PINMOTORPWM2 12

PID PID(5, 1.2, 0.5);

int analogPins[] = {A5, A6, A7, A8, A9, A10, A11, A12, A13, A14};
float valorsensor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float valorsensorant[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int vel = 100;
int smooth = 1;
SensorCalibration cal(10, 80, 55);

float step = 0;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    cal.calibrate(2000, analogPins);
    delay(3000);
    for (int i = 0; i < 10; i++){
        valorsensor[i] = cal.getCalibratedValue(i, analogPins);
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
    long pt = micros();
    int error = 0;
    
    for (int i = 0; i < 10; i++)
    {
        
        valorsensor[i] = (cal.getCalibratedValue(i, analogPins))*0.9 + valorsensorant[i]*0.1;
        valorsensorant[i] = valorsensor[i];

        if (valorsensor[i] < 0.9)
        {
            valorsensor[i] = 1;
        }
        
    }
    for (int i = 0; i < 5; i++)
    {
        if (valorsensor[i] == 1)
        {
            error--;
        }
    }
    for (int i = 5; i < 10; i++)
    {
        if (valorsensor[i] == 1)
        {
            error++;
        }
    }

    Serial.println(error);
    float output = PID.pid(error, step);
        digitalWrite(PINMOTOR1A, LOW);
        digitalWrite(PINMOTOR1B, HIGH);
        digitalWrite(PINMOTOR2A, LOW);
        digitalWrite(PINMOTOR2B, HIGH);
    if (output > 0)
    {
        

        analogWrite(PINMOTORPWM1, vel - output * vel);
        analogWrite(PINMOTORPWM2, vel);
    }
    else
    {
        analogWrite(PINMOTORPWM1, vel);
        analogWrite(PINMOTORPWM2, vel - abs(output) * vel);
    }


    unsigned long step_micros = micros() - pt;
    step = (float)step_micros / 1000000;
}
