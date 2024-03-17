# --------------------------------------------------------------------------------
# File Name: main.py
# Authors: Federico Diaz Nemeth
# Date: 2024-01-06
# Description:
#   This Python code controls a robot's line following, calibration, and stopping
#   behaviors using a state machine approach. It communicates with an Arduino
#   board for motor control and sensor readings. The code employs PID control for
#   accurate line following and integrates sensor calibration for reliable readings.
# --------------------------------------------------------------------------------

# Import libraries
import timeit
from time import sleep
import pyfirmata2
from enum import Enum
import libs.Calibration  # Import Calibration library


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
# PID and Calibration objects
# --------------------------------------------------------------------------------
pid = libs.PID.PID(0.5, 0.5, 0.3)
calibration = libs.Calibration.Calibration(board, 10, 150, 90)

# --------------------------------------------------------------------------------
# Variable and pin definitions
# --------------------------------------------------------------------------------
currentState = RobotState.CALIBRATION  # Set the initial state

# Define the pins used for the motors and PWM signals
right_motor_bwd = 'd:8:o'
right_motor_fwd = 'd:9:o'
left_motor_bwd = 'd:10:o'
left_motor_fwd = 'd:11:o'
right_motor_pwm = 'd:13:p'
left_motor_pwm = 'd:12:p'

# Define the pins used for the sensors
analogPins = ['a:5:i', 'a:6:i', 'a:7:i', 'a:8:i', 'a:9:i', 'a:10:i', 'a:11:i', 'a:12:i', 'a:13:i', 'a:14:i']

# Define the variables used for the PID controller
sensorValues = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]  # Array to store the sensor values
baseSpeed = 120  # Base speed for the motors
step = 0  # Time step for the PID controller


# --------------------------------------------------------------------------------
# Functions
# --------------------------------------------------------------------------------
def err():
    # Function to calculate the error based on the sensor values
    error = 0
    for i in range(10):
        sensorValues[i] = calibration.getvalue(i, analogPins)
        if sensorValues[i] < 0.95:
            sensorValues[i] = 1
        if sensorValues[i] == 1:
            error += (2 if i <= 3 else 1) if i < 5 else -(2 if i >= 6 else 1)

    return error


def Avanzar(output):
    # Function to move the robot based on the output of the PID controller
    board.digital[right_motor_bwd].write(0)
    board.digital[right_motor_fwd].write(1)
    board.digital[left_motor_bwd].write(0)
    board.digital[left_motor_fwd].write(1)
    if output > 0:  # If the output is positive, turn right
        board.digital[right_motor_pwm].write(baseSpeed + output * baseSpeed)
        board.digital[left_motor_pwm].write(baseSpeed - output * baseSpeed)
    elif output < 0:  # If the output is negative, turn left
        board.digital[right_motor_pwm].write(baseSpeed - abs(output) * baseSpeed)
        board.digital[left_motor_pwm].write(baseSpeed + abs(output) * baseSpeed)
    else:  # If the output is zero, go straight
        board.digital[right_motor_pwm].write(baseSpeed)
        board.digital[left_motor_pwm].write(baseSpeed)


# --------------------------------------------------------------------------------
# State Machine functions
# --------------------------------------------------------------------------------
def idleState():
    print("Idle state")
    sleep(1)
    global currentState
    currentState = RobotState.LINE_FOLLOWING


def calibrationState():
    print("Calibration state")
    global sensorValues
    sensorValues = calibration.calibrate(2000, analogPins)  # Calibrate the sensors
    sleep(3)
    global currentState
    currentState = RobotState.IDLE


def lineFollowingState():
    global step
    # getting the step using timeit function to get the function running time in microseconds
    step = timeit.timeit(stmt='Avanzar(pid.calculate(err(), step))', globals=globals(), number=1) / 1000000


def stopState():
    print("Stop state")
    board.digital[right_motor_pwm].write(0)
    board.digital[left_motor_pwm].write(0)
    board.digital[right_motor_fwd].write(0)
    board.digital[left_motor_fwd].write(0)
    global currentState
    currentState = RobotState.IDLE


# --------------------------------------------------------------------------------
# Main loop
# --------------------------------------------------------------------------------
while True:
    match currentState:  # Switch statement to change the state of the robot
        case RobotState.IDLE:
            idleState()
        case RobotState.CALIBRATION:
            calibrationState()
        case RobotState.LINE_FOLLOWING:
            lineFollowingState()
        case RobotState.STOP:
            stopState()