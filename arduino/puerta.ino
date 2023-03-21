#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, 16, 2);

#define SS_PIN 53
#define RST_PIN 9

//-------Inicializacion pines sensor proximidad--------
int trigger = 10; // Pin digital 10 para el Trigger del sensor
int echo = 11;    // Pin digital 11 para el echo del sensor
long time = 0;
long dist = 0;
// trig 13
// echo 12

//-------Variable puerta abierta-----------
bool puertaAbierta;

MFRC522 rfid(SS_PIN, RST_PIN); // Instancia de la clase

MFRC522::MIFARE_Key mkey;
char d, d1, d2, d3, d10, d11, d12, d13;
int c, cpa, tag;
bool cp, np, ok, m;
// Inicializar array que almacenará el nuevo NUID
byte nuidPICC[4];

//-------configuración del teclado-----------------
const byte ROWS = 4; // cuatro filas
const byte COLS = 3; // tres columnas
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {2, 3, 4, 5}; // conectar a los pines de fila del teclado
byte colPins[COLS] = {6, 7, 8};    // conectar a los pines de columna del teclado

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//-------------declaración de funciones para el LCD---------------------

void setup()
{
    Serial.begin(9600);

    pinMode(54, OUTPUT);
    lcd.init();
    lcd.backlight();
    lcd.print("ESCANEE SU TARJETA");
    lcd.setCursor(0, 1);

    puertaAbierta = false;
    SPI.begin();     // Inicializar bus SPI
    rfid.PCD_Init(); // Inicializar MFRC522

    pinMode(trigger, OUTPUT); // Inicializar el pin del Trigger como salida del ultrasonido
    pinMode(echo, INPUT);    // Inicializar el pin del Echo como entrada del ultrasonido

    for (byte i = 0; i < 6; i++)
    {
        mkey.keyByte[i] = 0xFF;
    }

    Serial.println(F("Este código escanea el NUID de MIFARE Classic."));
    //--establecer contraseña predeterminada
    if (EEPROM.read(0) > 1 | EEPROM.read(0) < 1)
    {
        EEPROM.update(0, 1);
        EEPROM.update(1, '1');
        EEPROM.update(2, '2');
        EEPROM.update(3, '3');
        EEPROM.update(4, '4');
    }
    d = EEPROM.read(1);
    d1 = EEPROM.read(2);
    d2 = EEPROM.read(3);
    d3 = EEPROM.read(4);
}

void loop()
{
    /* if isDoorOpen dont run this code else display on lcd "puerta abierta" */

    if (isDoorOpen() == false)
    {


        //--------------------------código de sección de teclado----------------------
        char k = keypad.getKey();

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
    if (k == '#')
    {
        m = c = cp = np = tag = 0;
        lcd.clear();
        lcd.print("ESCANEE SU TARJETA");
    }

    if (c == 4 & np == 0)
    {
        c = 0;
        if (d10 == '*' & d11 == '0' & d12 == '0')
        {
            m = 1;
            lcd.clear();
            lcd.print("INGRESAR PASSWORD   ");
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
        if (d == d10 & d1 == d11 & d2 == d12 & d3 == d13)
        {
            if (cp == 0 & m == 0)
            {
                lcd.setCursor(0, 1);
                lcd.print("PASSWORD ACEPTADO");
                digitalWrite(54, HIGH);
                delay(3000);
                digitalWrite(54, LOW);
                d10 = d11 = d12 = d13 = 0;
                lcd.setCursor(0, 1);
                lcd.print("                ");
                lcd.setCursor(0, 1);
            }
            if (cp == 1 & tag == 0)
            {
                lcd.clear();
                lcd.print("NUEVO PASSWORD");
                lcd.setCursor(0, 1);
                np = 1;
            }
            if (tag > 0)
            {

                lcd.clear();
                lcd.print("ESCANEE SU TARJETA # ");
                lcd.print(tag);
                lcd.setCursor(0, 1);
            }
            m = 0;
        }
        else
        {
            if (!m)
            {
                lcd.setCursor(0, 1);
                lcd.print("PASSWORD INCORRECTO");
                delay(3000);
            }

            d10 = d11 = d12 = d13 = 0;
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
        }
    }

    if (c == 4 & np == 1)
    {
        m = 0;
        d = d10;
        d1 = d11;
        d2 = d12;
        d3 = d13;
        EEPROM.update(1, d);
        EEPROM.update(2, d1);
        EEPROM.update(3, d2);
        EEPROM.update(4, d3);
        np = 0;
        cp = 0;
        c = 0;
        d10 = d11 = d12 = d13 = 0;
        lcd.clear();
        lcd.print("ESCANEE SU TARJETA");
        lcd.setCursor(0, 1);
    }

    //---rfid----------------------
    // Restablezca el bucle si no hay ninguna tarjeta nueva presente en el sensor/lector. Esto guarda todo el proceso cuando está inactivo.
    if (!rfid.PICC_IsNewCardPresent())
        return;

    // Verificar si se ha leído la NUID
    if (!rfid.PICC_ReadCardSerial())
        return;
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    if (tag > 0)
    {
        switch (tag)
        {
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
        lcd.print("  ID DE ETIQUETA GUARDADA  ");

        delay(2000);
        lcd.clear();
        lcd.print("ESCANEE SU TARJETA");

        cp = 0;
        c = 0;
    }
    else
        for (int i = 1; i < 6; i++)
        {
            if (rfid.uid.uidByte[0] == EEPROM.read((i * 4) + 1) & rfid.uid.uidByte[1] == EEPROM.read((i * 4) + 2) & rfid.uid.uidByte[2] == EEPROM.read((i * 4) + 3) & rfid.uid.uidByte[3] == EEPROM.read((i * 4) + 4))
                ok = 1;
        }

    if (ok == 1)
    {
        ok = 0;
        Serial.println(F("Se ha detectado una nueva tarjeta."));
        lcd.setCursor(0, 1);
        lcd.print("TARJETA ACEPTADA ");
        digitalWrite(54, HIGH);
        delay(3000);
        lcd.setCursor(0, 1);
        lcd.print("                ");
        digitalWrite(54, LOW);
        return;
    }
    else
    {
        lcd.setCursor(0, 1);
        lcd.print(" ACCESO DENEGADO ");

        delay(2000);
        lcd.setCursor(0, 1);
        lcd.print("                ");
    }

    // Detener PICC
    rfid.PICC_HaltA();

    // Detener el cifrado en PCD
    rfid.PCD_StopCrypto1();


    } else {
        Serial.println("Door is open");
        return false;
    }
}
//------------------inicio de función de fin de ciclo---------------------
void printDec(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}


bool isDoorOpen(){
    /* using ultrasonic sensor check if distance is less than 2cm then is close */
    int distance = 0;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    distance = pulseIn(echoPin, HIGH);
    distance = distance / 58;
    if (distance < 2){
        /* serial print door is closed */
        Serial.println("Door is closed");
        return true;
    }
    /* serial print door is open */
    Serial.println("Door is open");
    return false;


}