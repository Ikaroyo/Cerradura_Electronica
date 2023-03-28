//----------------------Loop----------------------
void loopIn() {
  //--------------------------código de sección de teclado----------------------

  // Obtiene la tecla presionada
  char k = keypad.getKey();

  // Si la tecla es diferente de nula
  // Imprime por pantalla un asterisco y guarda el valor de la tecla en la variable k
  if (k) {
    c++;
    lcd.print("*");
    if (c == 1) {
      d10 = k;
    }
    if (c == 2) {
      d11 = k;
    }
    if (c == 3) {
      d12 = k;
    }
    if (c == 4) {
      d13 = k;
    }
  }

  // Si la tecla es igual a #, reinicia las variables y limpia la pantalla
  if (k == '#') {
    m = c = cp = np = tag = 0;
    lcd.clear();
    lcd.print("SCANEE TAG");
    lcd.setCursor(0, 1);
  }

  // si se han ingresado 4 caracteres y no se ha ingresado una nueva contraseña
  if (c == 4 & np == 0) {
    c = 0;

    // si la primer tecla es * y las siguientes son 00
    if (d10 == '*' & d11 == '0' & d12 == '0') {
      m = 1;
      lcd.clear();
      lcd.print("INGRESAR PWD   ");
      switch (d13) {
        case '0':
          cp = 1;
          break;
        case '1':
          tag = 1;
          break;
        case '2':
          tag = 2;
          break;
        case '3':
          tag = 3;
          break;
        case '4':
          tag = 4;
          break;
        case '5':
          tag = 5;
          break;
      }
      d10 = d11 = d12 = d13 = 0;
    }

    // comparar contraseña ingresada con la guardada en la memoria EEPROM
    if (d == d10 & d1 == d11 & d2 == d12 & d3 == d13) {

      // si la variable cp es igual a 0, la contraseña es correcta
      if (cp == 0 & m == 0) {
        // abrir puerta por tiempoApertura microsegundos
        lcd.setCursor(0, 1);
        lcd.print("PWD ACEPTADO");
        digitalWrite(54, HIGH);
        delay(tiempoApertura);
        digitalWrite(54, LOW);
        d10 = d11 = d12 = d13 = 0;
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
      }

      // si la variable cp es igual a 1, la contraseña es correcta y se solicita la nueva contraseña
      if (cp == 1 & tag == 0) {
        lcd.clear();
        lcd.print("NUEVO PWD");
        lcd.setCursor(0, 1);
        np = 1;
      }

      // si la variable tag es mayor a 0, se solicita el tag a guardar
      if (tag > 0) {
        lcd.clear();
        lcd.print("SCANEE TAG # ");
        lcd.print(tag);
        lcd.setCursor(0, 1);
      }
      m = 0;
    } else {
      // si la contraseña es incorrecta
      if (!m) {
        lcd.setCursor(0, 1);
        lcd.print("PWD INCORRECTO");
        delay(3000);
      }

      d10 = d11 = d12 = d13 = 0;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
    }
  }

  // si se han ingresado 4 caracteres y se solicita la nueva contraseña
  if (c == 4 & np == 1) {
    m = 0;
    d = d10;
    d1 = d11;
    d2 = d12;
    d3 = d13;

    // actualizar contraseña en la memoria EEPROM
    EEPROM.update(1, d);
    EEPROM.update(2, d1);
    EEPROM.update(3, d2);
    EEPROM.update(4, d3);
    np = 0;
    cp = 0;
    c = 0;
    d10 = d11 = d12 = d13 = 0;
    lcd.clear();
    lcd.print("SCANEE TAG");
    lcd.setCursor(0, 1);
  }

  //---rfid----------------------
  // Restablece el bucle si no hay ninguna TAG nueva presente en el sensor/lector. Esto guarda todo el proceso cuando está inactivo.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verificar si se ha leído la NUID
  if (!rfid.PICC_ReadCardSerial())
    return;

  // si la variable tag es mayor a 0, se solicita el tag a guardar
  if (tag > 0) {
    // actualizar tag en la memoria EEPROM
    switch (tag) {
        // si la variable tag es igual a 1, se guarda el tag en la posición 1
      case 1:
        EEPROM.update(5, rfid.uid.uidByte[0]);
        EEPROM.update(6, rfid.uid.uidByte[1]);
        EEPROM.update(7, rfid.uid.uidByte[2]);
        EEPROM.update(8, rfid.uid.uidByte[3]);
        break;
      case 2:
        EEPROM.update(9, rfid.uid.uidByte[0]);
        EEPROM.update(10, rfid.uid.uidByte[1]);
        EEPROM.update(11, rfid.uid.uidByte[2]);
        EEPROM.update(12, rfid.uid.uidByte[3]);
        break;
      case 3:
        EEPROM.update(13, rfid.uid.uidByte[0]);
        EEPROM.update(14, rfid.uid.uidByte[1]);
        EEPROM.update(15, rfid.uid.uidByte[2]);
        EEPROM.update(16, rfid.uid.uidByte[3]);
        break;
      case 4:
        EEPROM.update(17, rfid.uid.uidByte[0]);
        EEPROM.update(18, rfid.uid.uidByte[1]);
        EEPROM.update(19, rfid.uid.uidByte[2]);
        EEPROM.update(20, rfid.uid.uidByte[3]);
        break;
      case 5:
        EEPROM.update(21, rfid.uid.uidByte[0]);
        EEPROM.update(22, rfid.uid.uidByte[1]);
        EEPROM.update(23, rfid.uid.uidByte[2]);
        EEPROM.update(24, rfid.uid.uidByte[3]);
        break;
    }
    tag = 0;
    d10 = d11 = d12 = d13 = 0;
    lcd.setCursor(0, 1);
    lcd.print("  ID TAG GUARDADA  ");
    m = c = cp = np = tag = 0;
    delay(3000);
    lcd.clear();
    lcd.print("SCANEE TAG");
    lcd.setCursor(0, 1);

    cp = 0;
    c = 0;
  } else
    for (int i = 1; i < 6; i++) {
      // comparar el tag leído con los tags guardados en la memoria EEPROM
      if (rfid.uid.uidByte[0] == EEPROM.read((i * 4) + 1) & rfid.uid.uidByte[1] == EEPROM.read((i * 4) + 2) & rfid.uid.uidByte[2] == EEPROM.read((i * 4) + 3) & rfid.uid.uidByte[3] == EEPROM.read((i * 4) + 4))
        // cambiar el valor de la variable ok a 1 si el tag es correcto
        ok = 1;
    }

  // si el tag es correcto se abre la puerta
  if (ok == 1) {
    ok = 0;
    lcd.setCursor(0, 1);
    lcd.print("TAG ACEPTADA ");
    // abrir puerta por tiempoApertura microsegundos
    digitalWrite(54, HIGH);
    delay(tiempoApertura);
    m = c = cp = np = tag = 0;
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    digitalWrite(54, LOW);

    return;
  } else {
    lcd.setCursor(0, 1);
    lcd.print(" ACCESO DENEGADO ");
    m = c = cp = np = tag = 0;
    delay(2000);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
  }


  // Detener PICC
  rfid.PICC_HaltA();

  // Detener el cifrado en PCD
  rfid.PCD_StopCrypto1();
}
