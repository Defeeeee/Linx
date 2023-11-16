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
//     cal.calibrate(2000, analogPins);
//     delay(3000);
// }

// void loop()
// {
//     MoverConArray();
// }

// /**
//  * Rotates the robot in place in the specified direction.
//  * @param dir The direction of rotation. Positive values rotate clockwise, negative values rotate counterclockwise.
//  */
// void RotarEnLugar(int dir)
// {
//     if (dir > 0)
//     {
//         digitalWrite(PINMOTOR1A, LOW);
//         digitalWrite(PINMOTOR1B, HIGH);
//         digitalWrite(PINMOTOR2A, HIGH);
//         digitalWrite(PINMOTOR2B, LOW);
//         analogWrite(PINMOTORPWM1, vel * dir);
//         analogWrite(PINMOTORPWM2, vel * dir);
//     }
//     else if (dir < 0)
//     {
//         digitalWrite(PINMOTOR1A, HIGH);
//         digitalWrite(PINMOTOR1B, LOW);
//         digitalWrite(PINMOTOR2A, LOW);
//         digitalWrite(PINMOTOR2B, HIGH);
//         analogWrite(PINMOTORPWM1, vel * abs(dir));
//         analogWrite(PINMOTORPWM2, vel * abs(dir));
//     }
// }

// // Determines if the robot is on the line by checking the values of the sensor array.
// // Returns true if the number of sensors detecting the line is greater than 2, false otherwise.
// bool onLine()
// {
//     int n = 0;
//     for (int i = 0; i < 10; i++)
//     {
//         if (valorsensor[i] == 1)
//         {
//             n++;
//         }
//     }
//     return n > 2;
// }

// // Calculates the error based on the calibrated sensor values.
// // Returns the calculated error.
// int CalcErr()
// {
//     int error = 0;
//     for (int i = 0; i < 10; i++)
//     {
//         valorsensor[i] = cal.getCalibratedValue(i, analogPins);
//         if (valorsensor[i] < 0.95)
//         {
//             valorsensor[i] = 1;
//         }
//         if (i < 5 && valorsensor[i] == 1)
//         {
//             error--;
//         }
//         else if (i >= 5 && valorsensor[i] == 1)
//         {
//             error++;
//         }
//     }
//     return error;
// }

// /**
//  * Moves the robot forward while turning left or right based on the output value.
//  * @param output The output value that determines the direction and speed of the turn.
//  *               A positive value turns the robot right, a negative value turns it left, and 0 moves it straight.
//  */
// void Avanzar(float output)
// {
//     digitalWrite(PINMOTOR1A, LOW);
//     digitalWrite(PINMOTOR1B, HIGH);
//     digitalWrite(PINMOTOR2A, LOW);
//     digitalWrite(PINMOTOR2B, HIGH);
//     if (output > 0) // Turn Right
//     {
//         analogWrite(PINMOTORPWM1, vel - output * vel);
//         analogWrite(PINMOTORPWM2, vel);
//     }
//     else if (output < 0) // Turn Left
//     {
//         analogWrite(PINMOTORPWM1, vel);
//         analogWrite(PINMOTORPWM2, vel - abs(output) * vel);
//     }
//     else
//     {
//         analogWrite(PINMOTORPWM1, vel);
//         analogWrite(PINMOTORPWM2, vel);
//     }
// }

// /**
//  * Stops both motors by setting all pins to LOW and PWM to 0.
//  */
// void stop()
// {
//     digitalWrite(PINMOTOR1A, LOW);
//     digitalWrite(PINMOTOR1B, LOW);
//     digitalWrite(PINMOTOR2A, LOW);
//     digitalWrite(PINMOTOR2B, LOW);
//     analogWrite(PINMOTORPWM1, 0);
//     analogWrite(PINMOTORPWM2, 0);
// }

// /**
//  * Moves the robot according to an array of stops.
//  * Stops can be of different types:
//  * 0 - Move forward until an obstacle is detected.
//  * 1 - Rotate in place to the left until no obstacle is detected in front.
//  * 2 - Rotate in place to the right until no obstacle is detected in front.
//  * 3 - Do nothing and continue to the next stop.
//  */
// void MoverConArray()
// {
//     while (stops != 0)
//     {
//         long pt = micros();
//         Avanzar(PID.pid(CalcErr(), step) / 15.5);

//         step = (micros() - pt) / 1000000.0;


//         if (stop_type[stop_index] == 0 && !Ult.checkR(50))
//         {
//             stop_index++;
//             stops--;
//             continue;
//         }
//         else if (stop_type[stop_index] == 1 && Ult.checkF(30))
//         {
//             RotarEnLugar(1);
//             int aux = 0;
//             while (aux < 5)
//             {
//                 if (!Ult.checkF(60))
//                 {
//                     aux++;
//                 }
//             }
//             delay(50);
//             stop_index++;
//             continue;
//         }
//         else if (stop_type[stop_index] == 2 && !Ult.checkR(200))
//         {
//             delay(100);
//             RotarEnLugar(-1);
//             int aux = 0;
//             while (aux < 5)
//             {
//                 if (Ult.checkR(50))
//                 {
//                     aux++;
//                 }
//             }
//             stop_index++;
//             continue;
//         }
//         else if (stop_type[stop_index] == 3)
//         {
//             stop_index++;
//             stops--;
//             continue;
//         }
//     }
//     stop();
// }