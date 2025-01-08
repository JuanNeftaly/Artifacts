/*
  Programa que obtiene el UID de la tarjeta o llavero del kit RFID RC522, compara con
  valores previos obtenidos de UID y permite acceso con texto en monitor serie de bienvenida
  Cualquier otra tarjeta o llavero no listado muestra texto de No te conozco, es decir,
  acceso denegado. Requiere instalar libreria MFRC522
  Autor: bitwiseAr
*/

#include <SPI.h>     // incluye libreria bus SPI
#include <MFRC522.h> // incluye libreria especifica para MFRC522

#define RST_PIN D1 // constante para referenciar pin de reset
#define SS_PIN D2 // constante para referenciar pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

byte LecturaUID[4];                          // crea array para almacenar el UID leido
byte Usuario1[4] = {0xA2, 0x4A, 0xFF, 0x1C}; // UID de tarjeta
byte Usuario2[4] = {0xA2, 0x25, 0xE6, 0x1C};     // UID de llavero

void setup()
{
  Serial.begin(115200);                   // inicializa comunicacion por monitor serie
  SPI.begin();                            // inicializa bus SPI
  mfrc522.PCD_Init();                     // inicializa modulo lector
  Serial.println("RFID inicializado..."); // Muestra texto de inicialización
}

void loop()
{
  if (!mfrc522.PICC_IsNewCardPresent()) // si no hay una tarjeta presente
    return;                             // retorna al loop esperando por una tarjeta

  if (!mfrc522.PICC_ReadCardSerial()) // si no puede obtener datos de la tarjeta
    return;                           // retorna al loop esperando por otra tarjeta

  Serial.print("UID:"); // muestra texto UID:
  for (byte i = 0; i < mfrc522.uid.size; i++)
  { // bucle recorre de a un byte por vez el UID
    if (mfrc522.uid.uidByte[i] < 0x10)
    {                     // si el byte leido es menor a 0x10
      Serial.print(" 0"); // imprime espacio en blanco y numero cero
    }
    else
    {
      Serial.print(" "); // imprime un espacio en blanco
    }
    Serial.print(mfrc522.uid.uidByte[i], HEX); // imprime el byte del UID leido en hexadecimal
    LecturaUID[i] = mfrc522.uid.uidByte[i];    // almacena en array el byte del UID leido
  }

  Serial.print("\t"); // imprime un espacio de tabulacion

  if (comparaUID(LecturaUID, Usuario1))      // llama a funcion comparaUID con Usuario1
    Serial.println("Bienvenido Usuario 1");  // si retorna verdadero muestra texto bienvenida
  else if (comparaUID(LecturaUID, Usuario2)) // llama a funcion comparaUID con Usuario2
    Serial.println("Bienvenido Usuario 2");  // si retorna verdadero muestra texto bienvenida
  else
    Serial.println("No te conozco"); // muestra texto equivalente a acceso denegado

  mfrc522.PICC_HaltA(); // detiene comunicacion con tarjeta
}

boolean comparaUID(byte lectura[], byte usuario[]) // funcion para comparar UID
{
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {                               // bucle recorre de a un byte por vez el UID
    if (lectura[i] != usuario[i]) // si byte de UID leido es distinto a usuario
      return (false);             // retorna falso
  }
  return (true); // si los 4 bytes coinciden retorna verdadero
}
