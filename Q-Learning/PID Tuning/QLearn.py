from main import *
import numpy as np

states = ["far_left", "left", "center", "right", "far_right"]
actions = ["increase_P", "decrease_P", "increase_I", "decrease_I", "increase_D", "decrease_D", "reset_gains"]

Q = np.zeros((len(states), len(actions)))

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


def adjust_gains(action):
    # actions = ["increase_P", "decrease_P", "increase_I", "decrease_I", "increase_D", "decrease_D", "reset_gains"]

    global sensor_weights, P, I, D

    if action == "increase_P":
        P *= 1.1
    elif action == "decrease_P":
        P *= 0.9
    elif action == "increase_I":
        I *= 1.1
    elif action == "decrease_I":
        I *= 0.9
    elif action == "increase_D":
        D *= 1.1
    elif action == "decrease_D":
        D *= 0.9
    elif action == "reset_gains":
        P = 0.5
        I = 0.5
        D = 0.3

    log_pid_values(P, I, D)


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
    Q[current_state, action] += learning_rate * (
            reward + discount_factor * np.max(Q[next_state]) - Q[current_state, action])
