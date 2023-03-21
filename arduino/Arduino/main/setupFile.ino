//----------------------Setup----------------------
void setupIn() {
  Serial.begin(9600);

  pinMode(54, OUTPUT);

  //--------Inicializacion LCD--------
  lcd.init();
  lcd.backlight();
  lcd.print("SCANEE TAG");
  lcd.setCursor(0, 1);

  //--------Inicializacion RFID--------
  SPI.begin();      // Inicializar bus SPI
  rfid.PCD_Init();  // Inicializar MFRC522

  //--------Inicializacion sensor proximidad--------
  pinMode(trigger, OUTPUT);  // Inicializar el pin del Trigger como salida del ultrasonido
  pinMode(echo, INPUT);      // Inicializar el pin del Echo como entrada del ultrasonido

  // vaciar el buffer de la tarjeta
  for (byte i = 0; i < 6; i++) {
    mkey.keyByte[i] = 0xFF;
  }

  //--------Establecer contraseña predeterminada--------
  if (EEPROM.read(0) > 1 | EEPROM.read(0) < 1) {
    EEPROM.update(0, 1);
    EEPROM.update(1, '1');
    EEPROM.update(2, '2');
    EEPROM.update(3, '3');
    EEPROM.update(4, '4');
  }

  //--------Obtener contraseña de la memoria EEPROM--------
  d = EEPROM.read(1);
  d1 = EEPROM.read(2);
  d2 = EEPROM.read(3);
  d3 = EEPROM.read(4);
}