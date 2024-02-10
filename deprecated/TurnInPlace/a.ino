// #include <SensorCalibration.h>
// #include <PIDv1.h>
// #include <Ultrasonico.h>

// #define PINMOTOR1A 8
// #define PINMOTOR1B 9
// #define PINMOTOR2A 10
// #define PINMOTOR2B 11
// #define trig2 2
// #define echo2 3
// #define trig1 4
// #define echo1 5

// #define PINMOTORPWM1 13
// #define PINMOTORPWM2 12

// #define wall_distance 50

// #define R1 22
// #define R2 23
// #define R3 24
// #define G1 25
// #define G2 26
// #define G3 27
// #define B1 28
// #define B2 29
// #define B3 30

// PID PID(3, 0.5, 0.3);
// Ultrasonico Ult(trig1, echo1, trig2, echo2);

// int analogPins[] = {A5, A6, A7, A8, A9, A10, A11, A12, A13, A14};
// float valorsensor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// int vel = 200;
// SensorCalibration cal(10, 150, 90);

// float step = 0;
// int stop_index = 0;

// int stops = 50;

// int stop_type[] = { 1, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 2, 1, 2, 0, 0, 1, 1, 0, 0, 1, 2, 3, 0, 0, 0}; // 0 = aula, 1 = izquierda, 2 = derecha, 3 = ?

// void setup()
// {
//     Serial.begin(115200);
// }

// void loop()
// {
//           Serial.println("CheckF: ");
//         Serial.println(Ult.distanciaF());
//         Serial.println("CheckR: ");
//         Serial.println(Ult.distanciaR());
//         Serial.println("");
// }

