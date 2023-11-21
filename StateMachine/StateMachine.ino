#include <SensorCalibration.h>
#include <PIDv1.h>
#include <Ultrasonico.h>

/**
 * @file StateMachine.ino
 * @brief This file contains the implementation of a state machine for a robot.
 * The robot has different states such as IDLE, CALIBRATION, LINE_FOLLOWING, and STOP.
 * It uses sensor calibration, PID control, and ultrasonic sensors for line following and obstacle detection.
 * The robot can be controlled through serial communication.
 * 
 * @author Manuel Rao, Federico Diaz Nemeth
 * 
 * @details
 * The state machine transitions between different states based on the current state and input received through serial communication.
 * The robot starts in the CALIBRATION state where it calibrates the sensors.
 * After calibration, it transitions to the IDLE state where it waits for user input to start line following.
 * In the LINE_FOLLOWING state, the robot follows a line using PID control and detects obstacles using ultrasonic sensors.
 * It stops at predefined stops based on the stop type and wall distance.
 * Once all stops are completed, it transitions to the STOP state where it stops the motors and returns to the IDLE state.
 * 
 * The robot uses the following pins for motor control and sensor connections:
 * - Motor 1A: Pin 8
 * - Motor 1B: Pin 9
 * - Motor 2A: Pin 10
 * - Motor 2B: Pin 11
 * - Ultrasonic Sensor 1 Trigger: Pin 4
 * - Ultrasonic Sensor 1 Echo: Pin 5
 * - Ultrasonic Sensor 2 Trigger: Pin 2
 * - Ultrasonic Sensor 2 Echo: Pin 3
 * - Motor 1 PWM: Pin 13
 * - Motor 2 PWM: Pin 12
 * 
 * The robot uses a PID controller for line following, with the following PID constants:
 * - Proportional Gain: 3
 * - Integral Gain: 0.5
 * - Derivative Gain: 0.3
 * 
 * The robot also uses sensor calibration to calibrate the sensor values.
 * It uses 10 analog pins for sensor readings and stores the calibrated values in an array.
 * 
 * The robot has a total of 5 stops, each with a stop type and wall distance.
 * The stop types are defined as follows:
 * - 0: Aula
 * - 1: Left
 * - 2: Right
 * - 3: Unknown
 * 
 * The wall distances are defined for each stop and determine when the robot should stop or change direction.
 * 
 * The robot's main loop checks for serial input and transitions between states based on the input and current state.
 * The robot also has helper functions for each state and motor control.
 * 
 * @note This code is for illustrative purposes and may require additional modifications for specific robot configurations.
 */

// States
enum RobotState { 
    IDLE,
    CALIBRATION,
    LINE_FOLLOWING,
    STOP
};

// Current state
RobotState currentState = CALIBRATION;

// Pin definitions
#define PINMOTOR1A 8
#define PINMOTOR1B 9
#define PINMOTOR2A 10
#define PINMOTOR2B 11

#define trig2 5
#define echo2 4
#define trig1 3
#define echo1 2

#define PINMOTORPWM1 13
#define PINMOTORPWM2 12

#define pinLedrojo1 = 30;
#define pinLedverde1 = 32;
#define pinLedazul1 = 34;
#define pinLedazul2 = 40;
#define pinLedverde2 = 41;
#define pinLedrojo2 = 42;
#define pinLedazul3 = 43;
#define pinLedrojo3 = 44;
#define pinLedverde3 = 45;

int analogPins[] = {A5, A6, A7, A8, A9, A10, A11, A12, A13, A14};

// PID controller, ultrasonic sensor, and sensor calibration objects
PID PID(3, 0.5, 0.3);
Ultrasonico Ult(trig1, echo1, trig2, echo2);
SensorCalibration cal(10, 150, 90);

// Sensor values storing arrays
float valorsensor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Motor velocity
int vel = 200;

float step = 0; // Time step
int stop_index = 0; // Current stop index

int stops = 50; // Total number of stops

int current_stop = 0; // Current stop

int stop_type[] = { 1, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 2, 1, 2, 0, 0, 1, 1, 0, 0, 1, 2, 3, 0, 0, 0}; // 0 = aula, 1 = izquierda, 2 = derecha, 3 = ?
int wall_distance[] = { 30, 150, 150, 150, 150, 150, 250, 30, 150, 150, 150, 150, 150, 30, 150, 150, 150, 50, 30, 150, 150, 30, 150, 200, 150, 150, 150}; // Distancia de la pared

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    // Check for serial input
    // if (Serial.available()) {
    //     char input = Serial.read();
    //     if (input == "stop") {
    //         currentState = STOP;
    //     }
    // }

    // State machine
    switch (currentState) {
        case IDLE:
            idleState();
            break;
        case CALIBRATION:
            calibrationState();
            break;
        case LINE_FOLLOWING:
            lineFollowingState();
            break;
        case STOP:
            StopState();
            break;
    }
}

// State functions
void idleState() {
    if (Serial.available()) {
        int input = Serial.parseInt();
        stops = input;
        // stops = input - current_stop;
        // if (stops < 0) {
        //     stops = stops + 15;
        // }
        // current_stop = stops + current_stop;
        // if (current_stop > 15) {
        //     current_stop = current_stop - 15;
        // }
        currentState = LINE_FOLLOWING;
    }
}

void calibrationState() {
    Serial.println("Calibrating...");
    cal.calibrate(2000, analogPins);
    delay(3000);
    currentState = IDLE;
}

void lineFollowingState() {
    digitalWrite(pinLedrojo1, LOW);
    digitalWrite(pinLedrojo2, LOW);
    digitalWrite(pinLedrojo3, LOW);
    digitalWrite(pinLedazul1, LOW);
    digitalWrite(pinLedazul2, LOW);
    digitalWrite(pinLedazul3, LOW);

    digitalWrite(pinLedverde1, HIGH);
    digitalWrite(pinLedverde2, HIGH);
    digitalWrite(pinLedverde3, HIGH);
    Serial.println("Line following...");
    if (stops > 0) {

        long pt = micros();
        Avanzar(PID.pid(CalcErr(), step) / 15.5);
        step = (micros() - pt) / 1000000.0;

        switch (stop_type[stop_index]) {
            case 0:
                if (!Ult.checkR(wall_distance[stop_index])) {
                    stops--;
                    stop_index++;
                    }
                break;
            case 1:
                if (Ult.checkF(wall_distance[stop_index])) {
                    int aux = 0;
                    RotarEnLugar(1);
                    while (aux < 3) {
                        if (!Ult.checkF(100)) {
                            aux++;
                        }
                    }
                    stop_index++;
                }
                break;
            case 2:
                if (!Ult.checkR(wall_distance[stop_index])) {
                    delay(750);
                    RotarEnLugar(-1);
                    delay(1000);
                    stop_index++;
                }
                break;
            case 3:
                stop_index++;
                break;
        }
    } else {
        currentState = STOP;
    }
}

void StopState()
{
    // PRENDO LEDS ROJOS
    digitalWrite(pinLedrojo1, HIGH);
    digitalWrite(pinLedrojo2, HIGH);
    digitalWrite(pinLedrojo3, HIGH);
    // APAGO EL RESTO DE LEDS
    digitalWrite(pinLedverde1, LOW);
    digitalWrite(pinLedverde2, LOW);
    digitalWrite(pinLedverde3, LOW);
    digitalWrite(pinLedazul1, LOW);
    digitalWrite(pinLedazul2, LOW);
    digitalWrite(pinLedazul3, LOW);

    digitalWrite(PINMOTOR1A, LOW);
    digitalWrite(PINMOTOR1B, LOW);
    digitalWrite(PINMOTOR2A, LOW);
    digitalWrite(PINMOTOR2B, LOW);
    analogWrite(PINMOTORPWM1, 0);
    analogWrite(PINMOTORPWM2, 0);
    currentState = IDLE;
}

void RotarEnLugar(int dir)
{
    if (dir > 0)
    {
        digitalWrite(PINMOTOR1A, LOW);
        digitalWrite(PINMOTOR1B, HIGH);
        digitalWrite(PINMOTOR2A, HIGH);
        digitalWrite(PINMOTOR2B, LOW);
        analogWrite(PINMOTORPWM1, vel * dir);
        analogWrite(PINMOTORPWM2, vel * dir);
    }
    else if (dir < 0)
    {
        digitalWrite(PINMOTOR1A, HIGH);
        digitalWrite(PINMOTOR1B, LOW);
        digitalWrite(PINMOTOR2A, LOW);
        digitalWrite(PINMOTOR2B, HIGH);
        analogWrite(PINMOTORPWM1, vel * abs(dir));
        analogWrite(PINMOTORPWM2, vel * abs(dir));
    }
}

// Calculates the error based on the calibrated sensor values.
// Returns the calculated error.
int CalcErr()
{
    int error = 0;
    for (int i = 0; i < 10; i++)
    {
        valorsensor[i] = cal.getCalibratedValue(i, analogPins);
        if (valorsensor[i] < 0.95)
        {
            valorsensor[i] = 1;
        }
        if (i < 5 && valorsensor[i] == 1)
        {
            error--;
        }
        else if (i >= 5 && valorsensor[i] == 1)
        {
            error++;
        }
    }
    return error;
}

// Controls the motors based on the PID output.
void Avanzar(float output)
{
    digitalWrite(PINMOTOR1A, LOW);
    digitalWrite(PINMOTOR1B, HIGH);
    digitalWrite(PINMOTOR2A, LOW);
    digitalWrite(PINMOTOR2B, HIGH);
    if (output > 0) // Turn Right
    {
        analogWrite(PINMOTORPWM1, vel - output * vel);
        analogWrite(PINMOTORPWM2, vel);
    }
    else if (output < 0) // Turn Left
    {
        analogWrite(PINMOTORPWM1, vel);
        analogWrite(PINMOTORPWM2, vel - abs(output) * vel);
    }
    else
    {
        analogWrite(PINMOTORPWM1, vel);
        analogWrite(PINMOTORPWM2, vel);
    }
}