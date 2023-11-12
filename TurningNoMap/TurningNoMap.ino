#include <SensorCalibration.h>
#include <PIDv1.h>
#include <Ultrasonico.h>

#define PINMOTOR1A 8
#define PINMOTOR1B 9
#define PINMOTOR2A 10
#define PINMOTOR2B 11
#define trig2 2
#define echo2 3
#define trig1 4
#define echo1 5

#define PINMOTORPWM1 13
#define PINMOTORPWM2 12

#define wall_distance 50

#define R1 22
#define R2 23
#define R3 24
#define G1 25
#define G2 26
#define G3 27
#define B1 28
#define B2 29
#define B3 30

PID PID(3, 0.5, 0.3);
Ultrasonico Ult(trig1, echo1, trig2, echo2);

int analogPins[] = {A5, A6, A7, A8, A9, A10, A11, A12, A13, A14}; // 10 sensores
float valorsensor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int vel = 255;                      // 0-255
SensorCalibration cal(10, 150, 90); // 10 sensores, velocidad "80" de impulso, 55 de constante

float step = 0; // tiempo de cada iteracion
int stop_index = 0;

int stops = 50;

int stop_type[] = {1, 1, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 2, 1, 2, 0, 0, 1, 1, 0, 0, 1, 2, 3, 0, 0, 0}; // 0 = aula, 1 = izquierda, 2 = derecha, 3 = ?


void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    cal.calibrate(2000, analogPins); // 2000 iteraciones de calibracion
    delay(3000);
}

void loop()
{
    MoverConArray(); // mueve el robot en funcion a un array de paradas y sensores
}

bool onLine()
{
    int n = 0;
    for (int i = 0; i < 10; i++)
    {
        if (valorsensor[i] == 1)
        {
            n++;
        }
    }
    if (n > 2)
    {
        return true;
    }
    else
    {
        return false;
    }
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

void stop()
{
    digitalWrite(PINMOTOR1A, LOW);
    digitalWrite(PINMOTOR1B, LOW);
    digitalWrite(PINMOTOR2A, LOW);
    digitalWrite(PINMOTOR2B, LOW);
    analogWrite(PINMOTORPWM1, 0);
    analogWrite(PINMOTORPWM2, 0);
}

void MoverConArray()
{
    while (stops != 0)
    {
        long pt = micros();
        Avanzar(PID.pid(CalcErr(), step) / 15.5);

        if (stop_type[stop_index] == 0 && !checkR(50)) {
            stop_index++;
            stops--;
            continue;
        }
        else if (stop_type[stop_index] == 1 && checkF(50)) {
            Avanzar(-1);
            int aux = 0;
            while (aux < 5) {
                if (!checkF(50)) {
                    aux++;
                }
                delay(5);
            }
            stop_index++;
            stops--;
            continue;
        } else if (stop_type[stop_index] == 2 && !checkR(100)) {
            stop_index++;
            stops--;
            continue;
        } else if (stop_type[stop_index] == 3) {
            stop_index++;
            stops--;
            continue;
        }
        step = (micros() - pt) / 1000000.0;
    }
    stop();
}