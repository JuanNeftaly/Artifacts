#include <SPI.h>
#include <LoRa.h>

int counter = 0;

#define SS      10
#define RST     5
#define DI0     2

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");
    
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("UCA "); // Coloca un identificador del mensaje aca, ej. G1
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(1000);
}
