#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//-----------------INICIO INICIALIZACION-----------------

LiquidCrystal_I2C lcd(0x3f, 16, 2);  // Dirección I2C del LCD y tamaño de la pantalla

//-------Asignacion de pines Sensor RFID--------
#define SS_PIN 53
#define RST_PIN 9

//--------Inicializacion pines sensor proximidad--------
int trigger = 10;  // Pin digital 10 para el Trigger del sensor
int echo = 11;     // Pin digital 11 para el echo del sensor

//--------Variable para modificar el LCD una sola vez--------
bool LCDAbierta = false;
bool LCDCerrada = true;

MFRC522 rfid(SS_PIN, RST_PIN);  // Instancia de la clase

MFRC522::MIFARE_Key mkey;
char d, d1, d2, d3, d10, d11, d12, d13;  // d: primer digito de la contraseña, d1: segundo digito de la contraseña, d2: tercer digito de la contraseña, d3: cuarto digito de la contraseña, d10: primer digito de la contraseña ingresada, d11: segundo digito de la contraseña ingresada, d12: tercer digito de la contraseña ingresada, d13: cuarto digito de la contraseña ingresada
int c, cpa, tag;                         // c: contador de caracteres, cpa: contador de caracteres para cambiar contraseña, tag: numero de tag
bool cp, np, ok, m;                      // cp: cambiar contraseña, np: nueva contraseña, ok: contraseña correcta, m: modo

// Inicializar array que almacenará el nuevo NUID
byte nuidPICC[4];

//--------configuración del teclado--------
const byte ROWS = 4;  // cuatro filas
const byte COLS = 3;  // tres columnas

// asignacion de los valores de los botones del teclado
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { 2, 3, 4, 5 };  // conectar a los pines de fila del teclado
byte colPins[COLS] = { 6, 7, 8 };     // conectar a los pines de columna del teclado

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

long tiempoApertura = 5000;  // tiempo de apertura de la puerta en milisegundos

//-----------------FIN INICIALIZACION-----------------



void setup() {
  setupIn();
}

void loop() {

  //--------Comprobacion de puerta abierta--------
  // Si la puerta esta abierta no solicitará contraseña ni tag

  if (!isDoorOpen()) {
    loopIn();
  }
}
