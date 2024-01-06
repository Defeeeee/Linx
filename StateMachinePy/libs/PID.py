class PID:
    def __init__(self, kp, ki, kd, clampie=True, maxie=1, minie=-1):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.clampie = clampie
        self.maxie = maxie
        self.minie = minie
        self.ie = 0
        self.de = 0
        self.lastError = 0

    def calculate(self, error, step):
        self.ie = self.ie + error
        self.de = (error - self.lastError) / step

        self.lastError = error

        if self.clampie:
            if self.ie > self.maxie:
                self.ie = self.maxie
            elif self.ie < self.minie:
                self.ie = self.minie

        return self.kp * error + self.ki * self.ie + self.kd * self.de
