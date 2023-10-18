void CalibrarIzq() {
  int aux = 0
  aux = analogRead(A5)
  if (aux > maxA) maxA = aux;
  if (aux < minA) minA = aux;

  aux = analogRead(A6)
  if (aux > maxB) maxB = aux;
  if (aux < minB) minB = aux;

  aux = analogRead(A7)
  if (aux > maxC) maxC = aux;
  if (aux < minC) minC = aux;

  aux = analogRead(A8)
  if (aux > maxD) maxD = aux;
  if (aux < minD) minD = aux;

  aux = analogRead(A9)
  if (aux > maxE) maxE = aux;
  if (aux < minE) minE = aux;
}

void CalibrarDer() {
  int aux = 0
  aux = analogRead(A10)
  if (aux > maxF) maxF = aux;
  if (aux < minF) minF = aux;

  aux = analogRead(A11)
  if (aux > maxG) maxG = aux;
  if (aux < minG) minG = aux;

  aux = analogRead(A12)
  if (aux > maxH) maxH = aux;
  if (aux < minH) minH = aux;

  aux = analogRead(A13)
  if (aux > maxI) maxI = aux;
  if (aux < minI) minI = aux;

  aux = analogRead(A14)
  if (aux > maxJ) maxJ = aux;
  if (aux < minBJ) minJ = aux;
}