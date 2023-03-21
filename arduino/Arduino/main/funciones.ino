
//----------------------funciones----------------------
// funcion para imprimir el UID de la tarjeta en el monitor serial (opcional)
void printDec(byte *buffer, byte bufferSize) {
  // Imprimir el número decimal (UID) de la tarjeta
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

// funcion para controlar si la puerta esta abierta o cerrada
bool isDoorOpen() {
  // variables para el sensor de ultrasonidos
  long time = 0;
  int distance = 0;

  // usando el sensor de ultrasonidos
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  time = pulseIn(echo, HIGH);

  distance = time / 58;

  // si la distancia es menor a 4 cm, la puerta esta cerrada
  if (distance < 4) {
    if (LCDCerrada == false) {
      closeDoorLCD();
    }
    return false;
  } else {
    if (LCDAbierta == false) {
      openDoorLCD();
    }
    return true;
  }
}

// funcion para mostrar en el LCD que la puerta esta abierta
void openDoorLCD() {
  lcd.clear();
  lcd.print("PUERTA ABIERTA");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  LCDAbierta = true;
  LCDCerrada = false;
}

// funcion para mostrar en el LCD que la puerta esta cerrada
void closeDoorLCD() {
  lcd.clear();
  lcd.print("SCANEE TAG");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  LCDCerrada = true;
  LCDAbierta = false;
}