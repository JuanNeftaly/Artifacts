#include <SPI.h>
#include <LoRa.h>

String incoming;

#define SS      D8
#define RST     D0
#define DI0     D1

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver");
  
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}


void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    incoming="";
   
    // read packet
    while (LoRa.available()) {
       incoming += (char)LoRa.read();
    }

    Serial.print("Received packet '");
    Serial.print(incoming);

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
