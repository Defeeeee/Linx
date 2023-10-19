void CalibrarIzq() {
  int aux = 0;
  aux = analogRead(A5);
  if (aux > MaxA) MaxA = aux;
  if (aux < MinA) MinA = aux;

  aux = analogRead(A6);
  if (aux > MaxB) MaxB = aux;
  if (aux < MinB) MinB = aux;

  aux = analogRead(A7);
  if (aux > MaxC) MaxC = aux;
  if (aux < MinC) MinC = aux;

  aux = analogRead(A8);
  if (aux > MaxD) MaxD = aux;
  if (aux < MinD) MinD = aux;

  aux = analogRead(A9);
  if (aux > MaxE) MaxE = aux;
  if (aux < MinE) MinE = aux;
}

void CalibrarDer() {
  int aux = 0;
  aux = analogRead(A10);
  if (aux > MaxF) MaxF = aux;
  if (aux < MinF) MinF = aux;

  aux = analogRead(A11);
  if (aux > MaxG) MaxG = aux;
  if (aux < MinG) MinG = aux;

  aux = analogRead(A12);
  if (aux > MaxH) MaxH = aux;
  if (aux < MinH) MinH = aux;

  aux = analogRead(A13);
  if (aux > MaxI) MaxI = aux;
  if (aux < MinI) MinI = aux;

  aux = analogRead(A14);
  if (aux > MaxJ) MaxJ = aux;
  if (aux < MinJ) MinJ = aux;
}

void Mapear() {
  val
  valA = map()
}