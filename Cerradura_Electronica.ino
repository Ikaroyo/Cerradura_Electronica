#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2); // Dirección I2C del LCD y tamaño de la pantalla

//-------Asignacion de pines Sensor RFID--------
#define SS_PIN 53
#define RST_PIN 9

//--------Inicializacion pines sensor proximidad--------
int trigger = 10; // Pin digital 10 para el Trigger del sensor
int echo = 11;    // Pin digital 11 para el echo del sensor

//--------Variable para modificar el LCD una sola vez--------
bool LCDAbierta = false;
bool LCDCerrada = true;

MFRC522 rfid(SS_PIN, RST_PIN); // Instancia de la clase

MFRC522::MIFARE_Key mkey;
char d, d1, d2, d3, d10, d11, d12, d13; // d: primer digito de la contraseña, d1: segundo digito de la contraseña, d2: tercer digito de la contraseña, d3: cuarto digito de la contraseña, d10: primer digito de la contraseña ingresada, d11: segundo digito de la contraseña ingresada, d12: tercer digito de la contraseña ingresada, d13: cuarto digito de la contraseña ingresada
int c, cpa, tag;                        // c: contador de caracteres, cpa: contador de caracteres para cambiar contraseña, tag: numero de tag
bool cp, np, ok, m;                     // cp: cambiar contraseña, np: nueva contraseña, ok: contraseña correcta, m: modo

// Inicializar array que almacenará el nuevo NUID
byte nuidPICC[4];

//--------configuración del teclado--------
const byte ROWS = 4; // cuatro filas
const byte COLS = 3; // tres columnas

// asignacion de los valores de los botones del teclado
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {2, 3, 4, 5}; // conectar a los pines de fila del teclado
byte colPins[COLS] = {6, 7, 8};    // conectar a los pines de columna del teclado

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

long tiempoApertura = 5000; // tiempo de apertura de la puerta en milisegundos

void setup()
{

  Serial.begin(9600);

  pinMode(54, OUTPUT);

  //--------Inicializacion LCD--------
  lcd.init();
  lcd.backlight();
  lcd.print("SCANEE TAG");
  lcd.setCursor(0, 1);

  //--------Inicializacion RFID--------
  SPI.begin();     // Inicializar bus SPI
  rfid.PCD_Init(); // Inicializar MFRC522

  //--------Inicializacion sensor proximidad--------
  pinMode(trigger, OUTPUT); // Inicializar el pin del Trigger como salida del ultrasonido
  pinMode(echo, INPUT);     // Inicializar el pin del Echo como entrada del ultrasonido

  // vaciar el buffer de la tarjeta
  for (byte i = 0; i < 6; i++)
  {
    mkey.keyByte[i] = 0xFF;
  }

  //--------Establecer contraseña predeterminada--------
  if (EEPROM.read(0) > 1 | EEPROM.read(0) < 1)
  {
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

void loop()
{

  //--------Comprobacion de puerta abierta--------
  // Si la puerta esta abierta no solicitará contraseña ni tag

  if (!isDoorOpen())
  {

    //--------------------------código de sección de teclado----------------------

    // Obtiene la tecla presionada
    char k = keypad.getKey();

    // Si la tecla es diferente de nula
    // Imprime por pantalla un asterisco y guarda el valor de la tecla en la variable k
    if (k)
    {
      c++;
      Serial.println(k);

      lcd.print("*");
      if (c == 1)
      {
        d10 = k;
      }
      if (c == 2)
      {
        d11 = k;
      }
      if (c == 3)
      {
        d12 = k;
      }
      if (c == 4)
      {
        d13 = k;
      }
    }

    // Si la tecla es igual a #, reinicia las variables y limpia la pantalla
    if (k == '#')
    {
      m = c = cp = np = tag = 0;
      lcd.clear();
      lcd.print("SCANEE TAG");
      lcd.setCursor(0, 1);
    }

    // si se han ingresado 4 caracteres y no se ha ingresado una nueva contraseña
    if (c == 4 & np == 0)
    {
      c = 0;

      // si la primer tecla es * y las siguientes son 00
      if (d10 == '*' & d11 == '0' & d12 == '0')
      {
        m = 1;
        lcd.clear();
        lcd.print("INGRESAR PWD   ");
        switch (d13)
        {
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
      if (d == d10 & d1 == d11 & d2 == d12 & d3 == d13)
      {

        // si la variable cp es igual a 0, la contraseña es correcta
        if (cp == 0 & m == 0)
        {
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
        if (cp == 1 & tag == 0)
        {
          lcd.clear();
          lcd.print("NUEVO PWD");
          lcd.setCursor(0, 1);
          np = 1;
        }

        // si la variable tag es mayor a 0, se solicita el tag a guardar
        if (tag > 0)
        {
          lcd.clear();
          lcd.print("SCANEE TAG # ");
          lcd.print(tag);
          lcd.setCursor(0, 1);
        }
        m = 0;
      }
      else
      {
        // si la contraseña es incorrecta
        if (!m)
        {
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
    if (c == 4 & np == 1)
    {
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
    // Restablezca el bucle si no hay ninguna TAG nueva presente en el sensor/lector. Esto guarda todo el proceso cuando está inactivo.
    if (!rfid.PICC_IsNewCardPresent())
      return;

    // Verificar si se ha leído la NUID
    if (!rfid.PICC_ReadCardSerial())
      return;

    // Serial.print("UID tag :");
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    // si la variable tag es mayor a 0, se solicita el tag a guardar
    if (tag > 0)
    {
      // actualizar tag en la memoria EEPROM
      switch (tag)
      {
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
      delay(2000);
      lcd.clear();
      lcd.print("SCANEE TAG");
      lcd.setCursor(0, 1);

      cp = 0;
      c = 0;
    }
    else
      for (int i = 1; i < 6; i++)
      {
        // comparar el tag leído con los tags guardados en la memoria EEPROM
        if (rfid.uid.uidByte[0] == EEPROM.read((i * 4) + 1) & rfid.uid.uidByte[1] == EEPROM.read((i * 4) + 2) & rfid.uid.uidByte[2] == EEPROM.read((i * 4) + 3) & rfid.uid.uidByte[3] == EEPROM.read((i * 4) + 4))
          // cambiar el valor de la variable ok a 1 si el tag es correcto
          ok = 1;
      }

    // si el tag es correcto se abre la puerta
    if (ok == 1)
    {
      ok = 0;
      Serial.println(F("Detectada nueva tag"));
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
    }
    else
    {
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
}

//----------------------funciones----------------------
// funcion para imprimir el UID de la tarjeta en el monitor serial (opcional)
void printDec(byte *buffer, byte bufferSize)
{
  // Imprimir el número decimal (UID) de la tarjeta
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

// funcion para controlar si la puerta esta abierta o cerrada
bool isDoorOpen()
{
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
  if (distance < 4)
  {
    if (LCDCerrada == false)
    {
      closeDoorLCD();
    }
    return false;
  }
  else
  {
    if (LCDAbierta == false)
    {
      openDoorLCD();
    }
    return true;
  }
}

// funcion para mostrar en el LCD que la puerta esta abierta
void openDoorLCD()
{
  lcd.clear();
  lcd.print("PUERTA ABIERTA");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  LCDAbierta = true;
  LCDCerrada = false;
}

// funcion para mostrar en el LCD que la puerta esta cerrada
void closeDoorLCD()
{
  lcd.clear();
  lcd.print("SCANEE TAG");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  LCDCerrada = true;
  LCDAbierta = false;
}
