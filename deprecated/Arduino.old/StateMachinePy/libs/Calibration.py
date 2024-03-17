from time import sleep

PINMOTOR1A = 'd:8:o'
PINMOTOR1B = 'd:9:o'
PINMOTOR2A = 'd:10:o'
PINMOTOR2B = 'd:11:o'

PINMOTORPWM1 = 'd:13:p'
PINMOTORPWM2 = 'd:12:p'


class Calibration:
    def __init__(self, board, numsensors, spd, pulsespd):
        self.board = board
        self.numsensors = numsensors
        self.spd = spd
        self.pulsespd = pulsespd
        self.sensorvalues = [0] * self.numsensors

    def calibrate(self, samples, analogpins):
        self.board.digital[PINMOTOR1A].write(0)
        self.board.digital[PINMOTOR1B].write(1)
        self.board.digital[PINMOTOR2A].write(0)
        self.board.digital[PINMOTOR2B].write(1)
        self.board.digital[PINMOTORPWM1].write(self.pulsespd)
        self.board.digital[PINMOTORPWM2].write(self.pulsespd)
        sleep(0.350)
        self.board.digital[PINMOTORPWM1].write(self.spd)
        self.board.digital[PINMOTORPWM2].write(self.spd)

        for i in range(self.numsensors):
            self.sensorvalues[i] = 0
            for j in range(samples):
                self.sensorvalues[i] += self.board.analog[analogpins[i]].read()
            self.sensorvalues[i] /= samples
            print("Sensor ", i, " value: ", self.sensorvalues[i])

        self.board.digital[PINMOTORPWM1].write(0)
        self.board.digital[PINMOTORPWM2].write(0)
        self.board.digital[PINMOTOR1B].write(0)
        self.board.digital[PINMOTOR2B].write(0)

        return self.sensorvalues

    def getvalue(self, index, analogpins):
        return self.board.analog[analogpins[index]].read() / self.sensorvalues[index]
