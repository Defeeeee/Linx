#include <Arduino.h>
#include <SensorCalibration.h>
#include <PIDv1.h>

#define PINMOTOR1A 2
#define PINMOTOR1B 3
#define PINMOTOR2A 4
#define PINMOTOR2B 5

#define PINMOTORPWM1 6
#define PINMOTORPWM2 7

SensorCalibration cal(10);

PID PID(1, 0.005, 0.2);

int analogPins[] = {A5, A6, A7, A8, A9, A10, A11, A12, A13, A14};
int valorsensor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int valDer = 0;
int valIzq = 0;

float step = 0;

void setup()
{
    // put your setup code here, to run once:
    cal.begin();
    cal.calibrate(2000, analogPins);
    Serial.begin(9600);
}

void loop()
{
    // put your main code here, to run repeatedly:
    long pt = micros();

    for (int i = 0; i < 10; i++)
    {
        valorsensor[i] = cal.getCalibratedValue(i);
        if (valorsensor[i] < 500)
        {
            valorsensor[i] = 1;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (valorsensor[i] == 1)
            valDer += 1;
    }
    for (int i = 5; i < 10; i++)
    {
        if (valorsensor[i] == 1)
            valIzq += 1;
    }

    float error = valDer - valIzq;
    float output = PID.pid(error, step);

    if (output > 0)
    {
        analogWrite(PINMOTORPWM1, output * 255);
        analogWrite(PINMOTORPWM2, 255);
    }
    else
    {
        analogWrite(PINMOTORPWM1, 255);
        analogWrite(PINMOTORPWM2, -output * 255);
    }


    unsigned long step_micros = micros() - pt;
    step = (float)step_micros / 1000000;
}
