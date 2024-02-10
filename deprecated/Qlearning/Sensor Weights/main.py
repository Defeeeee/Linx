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
import random
# Import libraries
import timeit
from time import sleep
import pyfirmata2
from enum import Enum
import libs.PID  # Import PID library
import libs.Calibration  # Import Calibration library
import numpy as np

states = ["far_left", "left", "center", "right", "far_right"]
actions = ["increase_left_weights", "increase_right_weights", "decrease_left_weights", "decrease_right_weights","reset_weights"]

Q = np.zeros((len(states), len(actions)))

sensor_weights = [5, 4, 3, 2, 1, -1, -2, -3, -4, -5]
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
def err(sens_val, sensWeight):
    # Function to calculate the error based on the sensor values
    error = 0
    # Sets a weight for each of the 10 sensors,being -5 the lowest weight and 5 the highest
    for i in range(10):
        sens_val[i] = calibration.getvalue(i, analogPins)
        if sens_val[i] < 0.95:
            sens_val[i] = 1
        if sens_val[i] == 1:
            error += sensWeight[i]

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


def determine_current_state():
    error = err(sensorValues, sensor_weights)
    if error < -5:  # Adjust thresholds as needed
        return "far_left"
    elif error < 0:
        return "left"
    elif error == 0:
        return "center"
    elif error > 0:
        return "right"
    else:
        return "far_right"


def select_action(Q, current_state):
    # Selects an action using epsilon greedy strateg
    epsilon = 0.1
    if random.random() < epsilon:
        return random.choice(actions)
    else:
        return np.argmax(Q[current_state])


def adjust_weights(action):

    global sensor_weights

    if action == "increase_left_weights":
        sensor_weights[0:5] *= 1.1  # Increase weights for left sensors
    elif action == "decrease_right_weights":
        sensor_weights[5:10] *= 0.9  # Decrease weights for right sensors
    elif action == "reset_weights":
        sensor_weights = [5, 4, 3, 2, 1, -1, -2, -3, -4, -5]  # Reset to default
    elif action == "increase_right_weights":
        sensor_weights[5:10] *= 1.1
    elif action == "decrease_left_weights":
        sensor_weights[0:5] *= 0.9


def rewarda(previous_state, current_state, next_state, error):
    reward = 0

    if abs(error) <= 2:
        reward += 2
        if current_state == "center":
            reward += 1
    if previous_state in ["turning_left", "turning_right"] and current_state == "center":
        reward += 5

    if current_state in ["far_left", "far_right"]:
        reward -= 2
    if abs(error) > 4:
        reward -= 2
    if previous_state == current_state and previous_state in ["turning_left", "turning_right"]:
        reward -= 1

    return reward


def update_q_value(Q, current_state, action, reward, next_state):
    learning_rate = 0.8
    discount_factor = 0.9
    Q[current_state, action] += learning_rate * (reward + discount_factor * np.max(Q[next_state]) - Q[current_state, action])


def lineFollowingState(previous_state=None):
    global step, sensor_weights
    current_state = determine_current_state()

    action = select_action(Q, current_state)
    adjust_weights(action)

    error = err(sensorValues, sensor_weights)
    # getting the step using timeit function to get the function running time in microseconds
    step = timeit.timeit(stmt='Avanzar(pid.calculate(error, step))', globals=globals(), number=1) / 1000000

    next_state = determine_current_state()

    reward = rewarda(previous_state, current_state, next_state, error)
    update_q_value(Q, current_state, action, reward, next_state)
    previous_state = current_state

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
