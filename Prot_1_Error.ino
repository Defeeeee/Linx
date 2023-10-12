
#include <PIDv1.h>

//defino pines puente H I/O y PWM
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

#define PWM2 8
#define PWM1 9

//defino arrays de sensores
int SensoresDerecha[] = {A0, A1, A2, A3, A4};
int SensoresIzquierda[] = {A5, A6, A7, A8, A9};

int ValorThr = 700;
float step = 0;
long lm = 0;

// Creo funcion Leer que toma por argumentos a los array de sensores y devuelve un error
int Leer(int sDer[], int sIzq[]) {
  int valorDer = 0;
  // Recorro el array de sensores en los dos lados y si miden mas que el valor threshold le sumo uno al "valor" de ese lado
  for (int i = 0; i < 5; i++) {
    if (analogRead(sDer[i]) < ValorThr) {
      valorDer++;
    }
  }

  int valorIzq = 0;
  for (int i = 0; i < 5; i++) {
    if (analogRead(sIzq[i]) < ValorThr) {
      valorIzq++;
    }
  }

  // devuelvo el error
  return valorIzq - valorDer;

}

PID PID(1, 0.05, 0.2);

void setup() {  for (int i = 0; i < 5; i++) {
    pinMode(SensoresDerecha[i], INPUT);
    pinMode(SensoresIzquierda[i], INPUT);
  }

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println(Leer(SensoresDerecha, SensoresIzquierda));
  delay(500);

  // long pt = micros();

  // int e = Leer(SensoresDerecha, SensoresIzquierda);

  // double pidOutput = PID.pid(e, step);

  // int motorSpeed1 = constrain(map(pidOutput, -255, 255, -255, 255), -255, 255);
  // int motorSpeed2 = constrain(map(pidOutput, -255, 255, -255, 255), -255, 255);

  // // Control the motors based on the PID output
  // if (pidOutput > 0) {
  //   // Turn left motor forward and right motor backward
  //   digitalWrite(IN1, HIGH);
  //   digitalWrite(IN2, LOW);
  //   digitalWrite(IN3, LOW);
  //   digitalWrite(IN4, HIGH);
  // } else {
  //   // Turn left motor backward and right motor forward
  //   digitalWrite(IN1, LOW);
  //   digitalWrite(IN2, HIGH);
  //   digitalWrite(IN3, HIGH);
  //   digitalWrite(IN4, LOW);
  // }

  // // Set motor speeds using PWM
  // analogWrite(PWM1, abs(motorSpeed1));
  // analogWrite(PWM2, abs(motorSpeed2));

  // unsigned long step_micros = micros() - pt;
  // step = (float)step_micros / 1000000;
}