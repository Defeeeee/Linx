# FEM (Line-Following Robot)

This project develops an autonomous robot that accurately follows a black line on the ground using an array of CNY70 sensors.

## Key Features

Line Sensing: Employs 10 CNY70 emitter-phototransistor sensors for reliable line detection.
PID Control: Uses a PID controller for smooth and accurate line-following adjustments.
Q-Learning: Implements Q-learning to dynamically optimize the PID controller gains for different track conditions.

### Hardware:
Arduino Mega 2560 board
10x CNY70 sensors
Motor control setup (H-bridge or motor driver shield)
Robot chassis, wheels, and power supply (battery)
### Software:
Arduino IDE and Jetbrains Pycharm

## How It Works

- Calibration: Before starting, the robot performs a sensor calibration routine to account for variations in sensor readings.
- Line Following: The robot reads the CNY70 sensor values and determines its position relative to the line.
- PID Control: The PID controller calculates steering adjustments based on the detected line position, ensuring the robot follows the line smoothly.
- Q-Learning Optimization: Through a trial-and-error process guided by rewards, the Q-learning algorithm learns the best combinations of PID gains for different situations (straight lines, turns, etc.).

## Project Structure (Qlearn Module)

- ```main.py```: Primary code file containing the state machine and core logic.
- ```QLearn.py```: Implements the Q-learning functionality.
- ```pins.py```: Defines Arduino pin assignments.
- ```libs/```: Contains supporting libraries for PID control, calibration, and motor control.

## Next Steps

Enhance line-following on complex courses: Improve detection and handling of curves, intersections, and varying line widths.
Error handling: Incorporate fail-safes and recovery mechanisms for sensor malfunctions or unexpected scenarios.
User interface: Consider adding LEDs for status indication or a way to monitor sensor data in real-time.
Contributions

### We welcome contributions to improve this project. Feel free to fork, submit pull requests, or raise issues.

- [@juanpanpanyz]( https://github.com/juanpanpanyz): - Juan Baader
- [@porfi2089](https://github.com/porfi2089): - Manuel Rao
- [@Defeeeee](https://github.com/Defeeeee): - Federico Diaz Nemeth
