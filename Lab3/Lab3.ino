//Importar las librerias a utilizar
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
//Inicializa la configuración de la pantalla
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

const int LM35 = A0; // Pin


void setup()   {
  Serial.begin(9600);
  
  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  display.setContrast (255); // dim display
 
  // Clear the buffer.
  display.clearDisplay();
  // Save to update screen changes
  display.display();
}

void loop() {
  
  // leer el dato 
  int Lectura = analogRead(LM35);

  // conversion de temperatura 
  float temperatura = Lectura * (5.0 / 1023.0) * 100;

  //Se borra la pantalla antes de finalizar el loop para que no se monten los datos
  display.clearDisplay();

  display.setTextSize(1); 
  display.setTextColor(SH110X_WHITE);

  display.setCursor(20,0);
  display.println("La Temp:");
  
  display.setCursor(20, 11);
  display.println(temperatura);

  //Para guardar los cambios hechos en la pantalla
  display.display();

  delay(1000);
}