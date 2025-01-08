#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>

#define RST_PIN D1
#define SS_PIN D2

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte LecturaUID[4];
byte Usuario1[4] = {0xA2, 0x4A, 0xFF, 0x1C};  // UID de tarjeta
byte Usuario2[4] = {0xA2, 0x25, 0xE6, 0x1C};

// Configuración WiFi
const char* ssid = "";
const char* password = "";

// Configuración MQTT
const char* mqtt_server = "io.adafruit.com" const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";

// Cliente WiFi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, mqtt_server, mqtt_port, mqtt_user,
			  mqtt_password);
Adafruit_MQTT_Publish rfidPublisher =
    Adafruit_MQTT_Publish(&mqtt, "rfid/bienvenida");

void setup() {
	Serial.begin(115200);
	SPI.begin();
	mfrc522.PCD_Init();
	Serial.println("RFID inicializado...");

	// Conexión WiFi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.println("Conectando a WiFi...");
	}
	Serial.println("Conectado a WiFi");

	// Conexión MQTT
	connectToMQTT();
}

void loop() {
	if (!mfrc522.PICC_IsNewCardPresent()) return;
	if (!mfrc522.PICC_ReadCardSerial()) return;

	String uid = "";
	for (byte i = 0; i < mfrc522.uid.size; i++) {
		if (mfrc522.uid.uidByte[i] < 0x10) {
			uid += " 0";
		} else {
			uid += " ";
		}
		uid += String(mfrc522.uid.uidByte[i], HEX);
		LecturaUID[i] = mfrc522.uid.uidByte[i];
	}

	Serial.print("UID:");
	Serial.println(uid);

	if (comparaUID(LecturaUID, Usuario1)) {
		Serial.println("Bienvenido Usuario 1");
		rfidPublisher.publish("Bienvenido Usuario 1");
	} else if (comparaUID(LecturaUID, Usuario2)) {
		Serial.println("Bienvenido Usuario 2");
		rfidPublisher.publish("Bienvenido Usuario 2");
	} else {
		Serial.println("No te conozco");
		rfidPublisher.publish("No te conozco");
	}

	mfrc522.PICC_HaltA();
}

boolean comparaUID(byte lectura[], byte usuario[]) {
	for (byte i = 0; i < mfrc522.uid.size; i++) {
		if (lectura[i] != usuario[i]) return false;
	}
	return true;
}

void connectToMQTT() {
	int8_t ret;
	while ((ret = mqtt.connect()) != 0) {
		Serial.println(mqtt.connectErrorString(ret));
		Serial.println("Reintentando conexión MQTT en 5 segundos...");
		mqtt.disconnect();
		delay(5000);
	}
	Serial.println("Conectado al broker MQTT");
}