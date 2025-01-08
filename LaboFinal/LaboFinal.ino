#include <ESP8266WiFi.h>
#include <LoRa.h>
#include <SPI.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

String incoming;

#define SS D8
#define RST D0
#define DI0 D1

/************************* WiFi Access Point *********************************/
#define WLAN_SSID ""
#define WLAN_PASS ""

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883  // use 8883 for SSL
#define AIO_USERNAME ""
#define AIO_KEY ""

/************ Global State (you don't need to change this!) ******************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and
// login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME,
			  AIO_KEY);

/****************************** Feeds for
 * Publishing***************************************/
// Setup a feed called 'reconocimiento' for publishing.
Adafruit_MQTT_Publish reconocimiento =
    Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/luztext");

void setup() {
	Serial.begin(115200);  // inicializa comunicacion por monitor serie

	// Conexión WiFi
	WiFi.begin(WLAN_SSID, WLAN_PASS);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("Conectado al WiFi");

	// Conexión MQTT
	if (mqtt.connect()) {
		Serial.println("Conectado a Adafruit IO");
	} else {
		Serial.println("Error conectándose a Adafruit IO");
	}

	while (!Serial);

	Serial.println("LoRa Receiver");

	LoRa.setPins(SS, RST, DI0);
	if (!LoRa.begin(915E6)) {
		Serial.println("Starting LoRa failed!");
		while (1);
	}
}

void loop() {
	MQTT_connect();
	// try to parse packet
	int packetSize = LoRa.parsePacket();
	if (packetSize) {
		// received a packet
		incoming = "";

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

	reconocimiento.publish(incoming.c_str());

	// Procesar los paquetes de MQTT para mantener la conexión con Adafruit
	// IO
	mqtt.processPackets(10);
	mqtt.ping();

	delay(10000);
}

void MQTT_connect() {
	int8_t ret;

	// Stop if already connected.
	if (mqtt.connected()) {
		return;
	}

	Serial.print("Connecting to MQTT... ");

	uint8_t retries = 3;
	while ((ret = mqtt.connect()) !=
	       0) {  // connect will return 0 for connected
		Serial.println(mqtt.connectErrorString(ret));
		Serial.println("Retrying MQTT connection in 10 seconds...");
		mqtt.disconnect();
		delay(10000);  // wait 10 seconds
		retries--;
		if (retries == 0) {
			// basically die and wait for WDT to reset me
			while (1);
		}
	}
	Serial.println("MQTT Connected!");
}
