# --------------------------------------------------------------------------------
# File Name: main.py
# Authors: Federico Diaz Nemeth
# Date: 2024-02-08
# Description:
#   This Python code controls a robot's line following, calibration, and stopping
#   behaviors using a state machine approach. It communicates with an Arduino
#   board for motor control and sensor readings. The code employs PID control for
#   accurate line following and integrates sensor calibration for reliable readings.
# --------------------------------------------------------------------------------
import random

import os.path
import csv
import shutil
import numpy as np

from QLearn import *
from pins import *
import timeit
from time import sleep
import pyfirmata2
from enum import Enum
import libs.PID as PID  # Import PID library
import libs.Calibration as Calibration  # Import Calibration library
import libs.HBridge as HBridge

# --------------------------------------------------------------------------------
# Define PID Gains & Sensor Weights
# --------------------------------------------------------------------------------

P = 0.5
I = 0.5
D = 0.3

sensor_weights = [5, 4, 3, 2, 1, -1, -2, -3, -4, -5]

# --------------------------------------------------------------------------------
# Saving and loading the Q-table
# --------------------------------------------------------------------------------

def save_q_table(Q, filename='q_table.csv'):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        for state in Q:
            writer.writerow([state] + Q[state].tolist())

def load_q_table(filename='q_table.csv'):
    Q = np.zeros((len(states), len(actions)))  # Initialize with your state/action dimensions
    with open(filename, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            state = row[0]
            Q[state] = np.array(list(map(float, row[1:])))
    return Q

# --------------------------------------------------------------------------------
# Retrieve PID values and q-table from the log file
# --------------------------------------------------------------------------------

# Load the last Q-table from the log file
if os.path.exists('q_table.csv'):
    Q = load_q_table()

# Load the last PID values from the log file
if os.path.exists('pid_log.csv'):
    with open('pid_log.csv', 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        last_row = None
        for row in reader:
            last_row = row
    if last_row:
        P, I, D = map(float, last_row)  # Read and convert to float values
    os.remove('pid_log.csv')  # Delete the log file after loading

# Recreate the log file with the last read PID values
if last_row:
    with open('pid_log.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(last_row)

# --------------------------------------------------------------------------------
# Define different states for the robot's behavior
# --------------------------------------------------------------------------------

class RobotState(Enum):
    IDLE = 0,
    CALIBRATION = 1,
    LINE_FOLLOWING = 2,
    STOP = 3


# --------------------------------------------------------------------------------
# Arduino setup
# --------------------------------------------------------------------------------

port = pyfirmata2.Arduino.AUTODETECT
board = pyfirmata2.Arduino(port)
board.samplingOn(1)  # Enable analog input sampling

# --------------------------------------------------------------------------------
# PID, HBridge and Calibration objects
# --------------------------------------------------------------------------------

pid = PID.PID(P, I, D)
calibration = Calibration.Calibration(board, 10, 150, 90)
motors = HBridge.Motors(board, left_motor_fwd, left_motor_bwd, right_motor_fwd, right_motor_bwd, left_motor_pwm,
                        right_motor_pwm)

# --------------------------------------------------------------------------------
# Variable and pin definitions
# --------------------------------------------------------------------------------

currentState = RobotState.CALIBRATION  # Set the initial state

# Define the variables used for the PID controller
sensorValues = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]  # Array to store the sensor values
baseSpeed = 120  # Base speed for the motors
step = 0  # Time step for the PID controller


# --------------------------------------------------------------------------------
# Functions
# --------------------------------------------------------------------------------

def log_pid_values(P, I, D):
    # Function to log the PID values to a CSV file
    with open('pid_log.csv', 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow([P, I, D])

def err(sens_val, sensweight):
    # Function to calculate the error based on the sensor values
    error = 0
    # Sets a weight for each of the 10 sensors,being -5 the lowest weight and 5 the highest
    for i in range(10):
        sens_val[i] = calibration.getvalue(i, analogPins)
        if sens_val[i] < 0.95:
            sens_val[i] = 1
        if sens_val[i] == 1:
            error += sensweight[i]

    return error


def Avanzar(output):
    # Function to move the robot based on the output of the PID controller
    motors.move(1)
    if output > 0:  # If the output is positive, turn right
        motors.speed(baseSpeed + output * baseSpeed, 1)
        motors.speed(baseSpeed - output * baseSpeed, 2)
    elif output < 0:  # If the output is negative, turn left
        motors.speed(baseSpeed + output * baseSpeed, 2)
        motors.speed(baseSpeed - output * baseSpeed, 1)
    else:  # If the output is zero, go straight
        motors.speed(baseSpeed, 3)


# --------------------------------------------------------------------------------
# State Machine functions
# --------------------------------------------------------------------------------

def idleState():
    print("Idle state")
    input("Iniciar")
    global currentState
    currentState = RobotState.LINE_FOLLOWING


def calibrationState():
    print("Calibration state")
    global sensorValues
    sensorValues = calibration.calibrate(2000, analogPins)  # Calibrate the sensors
    sleep(3)
    global currentState
    currentState = RobotState.IDLE

count = 0
def lineFollowingState(previous_state=None):
    global step, sensor_weights, Q, count
    count += 1
    current_state = determine_current_state()

    action = select_action(Q, current_state)
    adjust_gains(action)

    error = err(sensorValues, sensor_weights)
    # getting the step using timeit function to get the function running time in microseconds
    step = timeit.timeit(stmt='Avanzar(pid.calculate(error, step))', globals=globals(), number=1) / 1000000

    next_state = determine_current_state()

    reward = rewarda(previous_state, current_state, next_state, error)
    update_q_value(Q, current_state, action, reward, next_state)
    previous_state = current_state
    if count % 100 == 0:
        save_q_table(Q)
        count = 0


def stopState():
    global currentState
    print("Stop state")
    motors.move(0)
    currentState = RobotState.IDLE


# --------------------------------------------------------------------------------
# Main loop
# --------------------------------------------------------------------------------
while True:
    PID.PID(P, I, D)
    match currentState:  # Switch statement to change the state of the robot
        case RobotState.IDLE:
            idleState()
        case RobotState.CALIBRATION:
            calibrationState()
        case RobotState.LINE_FOLLOWING:
            lineFollowingState()
        case RobotState.STOP:
            stopState()
