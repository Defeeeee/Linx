class Motors:
    def __init__(self, board, pinl_fwd, pinl_bwd, pinr_fwd, pinr_bwd, pinl_pwm, pinr_pwm):
        self.board = board
        self.pinl_fwd = pinl_fwd
        self.pinl_bwd = pinl_bwd
        self.pinr_fwd = pinr_fwd
        self.pinr_bwd = pinr_bwd
        self.pinl_pwm = pinl_pwm
        self.pinr_pwm = pinr_pwm

    def move(self, direction):
        match direction:
            case 0:
                self.board.digital[self.pinr_bwd].write(0)
                self.board.digital[self.pinr_fwd].write(0)
                self.board.digital[self.pinl_bwd].write(0)
                self.board.digital[self.pinl_fwd].write(0)

            case 1:
                self.board.digital[self.pinr_bwd].write(0)
                self.board.digital[self.pinr_fwd].write(1)
                self.board.digital[self.pinl_bwd].write(0)
                self.board.digital[self.pinl_fwd].write(1)

            case -1:
                self.board.digital[self.pinr_bwd].write(1)
                self.board.digital[self.pinr_fwd].write(0)
                self.board.digital[self.pinl_bwd].write(1)
                self.board.digital[self.pinl_fwd].write(0)

    def speed(self, spd, motor):
        if not motor == 3:
            self.board.digital[self.pinl_pwm if motor == 1 else self.pinr_pwm].write(spd)
            return
        self.board.digital[self.pinl_pwm].write(spd)
        self.board.digital[self.pinr_pwm].write(spd)
