#include <SensorCalibration.h>
#include <PIDv1.h>

#define PINMOTOR1A 8
#define PINMOTOR1B 9
#define PINMOTOR2A 10
#define PINMOTOR2B 11

#define PINMOTORPWM1 13
#define PINMOTORPWM2 12

PID PID(3, 0.5, 0.3);

int analogPins[] = {A5, A6, A7, A8, A9, A10, A11, A12, A13, A14}; // 10 sensores
float valorsensor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int vel = 220;                      // 0-255
SensorCalibration cal(10, 150, 90); // 10 sensores, velocidad "80" de impulso, 55 de constante

float step = 0; // tiempo de cada iteracion

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    cal.calibrate(2000, analogPins); // 2000 iteraciones de calibracion
    delay(3000);
}

void loop()
{
    // put your main code here, to run repeatedly:
    long pt = micros();                       // tiempo de inicio de la iteracion
    int error = CalcErr();                     // calcula el error
    float output = PID.pid(error, step);           // calcula la salida del PID con el error y el tiempo de iteracion
    Avanzar(output);                           // mueve el robot
    unsigned long step_micros = micros() - pt; // calcula el tiempo de cada iteracion
    step = (float)step_micros / 1000000;
}

int CalcErr()
{
    int error = 0;
    for (int i = 0; i < 10; i++)
    {
        valorsensor[i] = cal.getCalibratedValue(i, analogPins); // obtiene el valor calibrado de cada sensor

        if (valorsensor[i] < 0.95) // si el valor es menor a 0.9, se considera que esta en la linea
        {
            valorsensor[i] = 1;
        }
    }
    for (int i = 0; i < 5; i++) // calcula el error del lado izquierdo
    {
        if (valorsensor[i] == 1)
        {
            error--;
        }
    }
    for (int i = 5; i < 10; i++) // calcula el error del lado derecho
    {
        if (valorsensor[i] == 1)
        {
            error++;
        }
    }
    return error;
}

void Avanzar(float output)
{
    digitalWrite(PINMOTOR1A, LOW);
    digitalWrite(PINMOTOR1B, HIGH);
    digitalWrite(PINMOTOR2A, LOW);
    digitalWrite(PINMOTOR2B, HIGH);
    if (output > 0) // si la salida es positiva, el motor izquierdo gira mas rapido
    {
        analogWrite(PINMOTORPWM1, vel - output * vel);
        analogWrite(PINMOTORPWM2, vel);
    }
    else if (output < 0) // si la salida es negativa, el motor derecho gira mas rapido
    {
        analogWrite(PINMOTORPWM1, vel);
        analogWrite(PINMOTORPWM2, vel - abs(output) * vel);
    }
    else
    { // si la salida es 0, ambos motores giran a la misma velocidad
        analogWrite(PINMOTORPWM1, vel);
        analogWrite(PINMOTORPWM2, vel);
    }
}