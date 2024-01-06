import timeit
from time import sleep
import pyfirmata2
from enum import Enum
import libs.PID
import libs.Calibration


class RobotState(Enum):
    IDLE = 0,
    CALIBRATION = 1,
    LINE_FOLLOWING = 2,
    STOP = 3


# Create a new board, specifying serial port
port = pyfirmata2.Arduino.AUTODETECT
board = pyfirmata2.Arduino(port)
board.samplingOn(1)

# Create a new PID controller
pid = libs.PID.PID(0.5, 0.5, 0.3)
calibration = libs.Calibration.Calibration(board, 10, 150, 90)

currentState = RobotState.CALIBRATION

PINMOTOR1A = 'd:8:o'
PINMOTOR1B = 'd:9:o'
PINMOTOR2A = 'd:10:o'
PINMOTOR2B = 'd:11:o'

PINMOTORPWM1 = 'd:13:p'
PINMOTORPWM2 = 'd:12:p'

analogPins = ['a:5:i', 'a:6:i', 'a:7:i', 'a:8:i', 'a:9:i', 'a:10:i', 'a:11:i', 'a:12:i', 'a:13:i', 'a:14:i']

sensorValues = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

baseSpeed = 120

step = 0


def err():
    error = 0
    for i in range(10):
        sensorValues[i] = calibration.getvalue(i, analogPins)
        if sensorValues[i] < 0.95:
            sensorValues[i] = 1
        if sensorValues[i] == 1:
            error += (2 if i <= 3 else 1) if i < 5 else -(2 if i >= 6 else 1)

    return error


def Avanzar(output):
    board.digital[PINMOTOR1A].write(0)
    board.digital[PINMOTOR1B].write(1)
    board.digital[PINMOTOR2A].write(0)
    board.digital[PINMOTOR2B].write(1)
    if output > 0:
        board.digital[PINMOTORPWM1].write(baseSpeed + output * baseSpeed)
        board.digital[PINMOTORPWM2].write(baseSpeed - output * baseSpeed)
    elif output < 0:
        board.digital[PINMOTORPWM1].write(baseSpeed - abs(output) * baseSpeed)
        board.digital[PINMOTORPWM2].write(baseSpeed + abs(output) * baseSpeed)
    else:
        board.digital[PINMOTORPWM1].write(baseSpeed)
        board.digital[PINMOTORPWM2].write(baseSpeed)


def idleState():
    print("Idle state")
    sleep(1)
    global currentState
    currentState = RobotState.LINE_FOLLOWING


def calibrationState():
    print("Calibration state")
    global sensorValues
    sensorValues = calibration.calibrate(2000, analogPins)
    sleep(3)
    global currentState
    currentState = RobotState.IDLE


def lineFollowingState():
    global step
    step = timeit.timeit(stmt='Avanzar(pid.calculate(err(), step))', globals=globals(), number=1) / 1000000


def stopState():
    print("Stop state")
    board.digital[PINMOTORPWM1].write(0)
    board.digital[PINMOTORPWM2].write(0)
    board.digital[PINMOTOR1B].write(0)
    board.digital[PINMOTOR2B].write(0)
    global currentState
    currentState = RobotState.IDLE


while True:
    match currentState:
        case RobotState.IDLE:
            idleState()
        case RobotState.CALIBRATION:
            calibrationState()
        case RobotState.LINE_FOLLOWING:
            lineFollowingState()
        case RobotState.STOP:
            stopState()
