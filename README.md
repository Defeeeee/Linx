# FEM (Line-Following Robot)

This project develops an autonomous robot that accurately follows a black line on the ground using an array of CNY70 sensors.

## How It Works

- Calibration: Before starting, the robot performs a sensor calibration routine to account for variations in sensor readings.
- Line Following: The robot reads the CNY70 sensor values and determines its position relative to the line.
- PID Control: The PID controller calculates steering adjustments based on the detected line position, ensuring the robot follows the line smoothly.
- Q-Learning Optimization: Through a trial-and-error process guided by rewards, the Q-learning algorithm learns the best combinations of PID gains for different situations (straight lines, turns, etc.).

### Hardware:
- Arduino Mega 2560 board
- 10x CNY70 sensors
- Motor control setup (H-bridge)
- Robot chassis(3d printed), wheels, and power supply (battery)
### Software:
- Arduino IDE, Jetbrains Pycharm and VScode

## Project Structure (Qlearn Module)

- ```main.py```: Primary code file containing the state machine and core logic.
- ```QLearn.py```: Implements the Q-learning functionality.
- ```pins.py```: Defines Arduino pin assignments.
- ```libs/```: Contains supporting libraries for PID control, calibration, and motor control.

## Next Steps

Enhance line-following on complex courses: Improve detection and handling of curves, intersections, and varying line widths.
Error handling: Incorporate fail-safes and recovery mechanisms for sensor malfunctions or unexpected scenarios.
User interface: we are considering adding a localy hosted webapp to be able to remotely call FEM.
FEM V2: we are prepareing a new version that incorporates machine vision to be able to navigate more complex terrain and go out of the line

Contributions

### We welcome contributions to improve this project. Feel free to fork, submit pull requests, or raise issues.

- [@juanpanpanyz]( https://github.com/juanpanpanyz): - Juan Baader
- [@porfi2089](https://github.com/porfi2089): - Manuel Rao
- [@Defeeeee](https://github.com/Defeeeee): - Federico Diaz Nemeth
