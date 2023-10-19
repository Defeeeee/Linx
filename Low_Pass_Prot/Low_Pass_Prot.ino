
#include <PIDv1.h>

//defino pines puente H I/O y PWM
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

#define PWM2 8
#define PWM1 9

//defino arrays de sensores
int SensoresIzquierda[] = {A10, A11, A12, A13, A14};
int SensoresDerecha[] = {A5, A6, A7, A8, A9};
int sensorval[] = {0,0,0,0,0,0,0,0,0,0};

int ValorThr = 250;
float step = 0;
long lm = 0;

// int MaxA = 0;
// int MaxB = 0;
// int MaxC = 0;
// int MaxD = 0;
// int MaxE = 0;
// int MaxF = 0;
// int MaxG = 0;
// int MaxH = 0;
// int MaxI = 0;
// int MaxJ = 0;

// int MinA = 1024;
// int MinB = 1024;
// int MinC = 1024;
// int MinD = 1024;
// int MinE = 1024;
// int MinF = 1024;
// int MinG = 1024;
// int MinH = 1024;
// int MinI = 1024;
// int MinJ = 1024;

// int valA;
// int valB;
// int valC;
// int valD;
// int valE;
// int valF;
// int valG;
// int valH;
// int valI;
// int valJ;

// Creo funcion Leer que toma por argumentos a los array de sensores y devuelve un error
int Leer(int sDer[], int sIzq[]) {
  int valorDer = 0;
  // Recorro el array de sensores en los dos lados y si miden mas que el valor threshold le sumo uno al "valor" de ese lado
  for (int i = 0; i < 5; i++) {
    sensorval[i] = analogRead(sDer[i])*0.2 + sensorval[i]*0.8; 
    if(sensorval[i] < ValorThr){
        valorDer++;
    }
    
  }

  int valorIzq = 0;
  for (int i = 5; i < 10; i++) {
    sensorval[i] = analogRead(sIzq[i-5])*0.2 + sensorval[i]*0.8; 
    if(sensorval[i] < ValorThr){
        valorIzq++;
    }
  }

  
  // devuelvo el error
  return valorIzq - valorDer;

}

PID PID(1, 0.005, 0.2);

void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(SensoresDerecha[i], INPUT);
    pinMode(SensoresIzquierda[i], INPUT);
  }

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  while (digitalRead(2)) {
    CalibrarIzq();
    CalibrarDer();
    Mapear();
  }
  
  long pt = micros();
  int e = Leer(SensoresDerecha, SensoresIzquierda);
  Serial.print(PID.pid(e, step));
  Serial.print(",");
  Serial.println(e);

  delay(5);

  unsigned long step_micros = micros() - pt;
  step = (float)step_micros / 1000000;
  
  
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