#include <DHT.h>
#include <ESP8266WiFi.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN D2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

/************************* WiFi Access Point *********************************/
#define WLAN_SSID ""
#define WLAN_PASS ""

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883  // use 8883 for SSL
#define AIO_USERNAME ""
#define AIO_KEY ""

/************ Global State (you don't need to change this!) ******************/
//  Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
// WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and
// login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME,
			  AIO_USERNAME, AIO_KEY);

/****************************** Feeds for Publishing*/
// Setup a feed called 'photocell' for publishing.
Adafruit_MQTT_Publish photocell =
    Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

Adafruit_MQTT_Publish sensor =
    Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/console");

// Feed para el infrarrojo
Adafruit_MQTT_Publish irsensor =
    Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ir-sensor");

/****************************** Feeds for Subscribing*/
// Setup a feed called 'slider' for subscribing to changes on the slider
Adafruit_MQTT_Subscribe slider =
    Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/slider", MQTT_QOS_1);

// Setup a feed called 'onoff' for subscribing to changes to the button
Adafruit_MQTT_Subscribe onoffbutton =
    Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff", MQTT_QOS_1);

/*************************** Sketch Code ************************************/
// Callbacks, funciones para el manejo de la información obtenida en adafruit
// (subscribed)
void slidercallback(double x) {
	Serial.print("Hey we're in a slider callback, the slider value is: ");
	Serial.println(x);
}

void onoffcallback(char *data, uint16_t len) {
	Serial.print("Hey we're in a onoff callback, the button value is: ");
	Serial.println(data);
}

/*********************************************** SetUp
 * *****************************/
void setup() {
	// Definicion del tipo de pines o cosas necesarias para el
	// funcionamiento de sensores
	pinMode(D0, INPUT);  // Pin del sensor infrarrojo

	// Iniciar la comunición Serial
	Serial.begin(115200);

	// Delay para comenzar la conexion WIFI
	delay(10);

	// Comenzamos el sensor DHT
	dht.begin();

	/****************************************** Connect to WiFi access point
	 * *************/
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(WLAN_SSID);

	WiFi.begin(WLAN_SSID, WLAN_PASS);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	/********************************************* Callback Functions for
	 * Subscribed Feeds *************/
	// Se setean los callbacks para las variables los cuales se ejecutaran
	// cuando se reciba información
	slider.setCallback(slidercallback);
	onoffbutton.setCallback(onoffcallback);

	// Configurar que MQTT se suscriba a los feed creados para subscribed
	mqtt.subscribe(&slider);
	mqtt.subscribe(&onoffbutton);
}

uint32_t x = 0;

//***************************************************** Loop
//********************************/
void loop() {
	// Ensure the connection to the MQTT server is alive (this will make the
	// first connection and automatically reconnect when disconnected).  See
	// the MQTT_connect function definition further below.
	MQTT_connect();

	// this is our 'wait for incoming subscription packets and callback em'
	// busy subloop try to spend your time here:
	mqtt.processPackets(10000);

	// ping the server to keep the mqtt connection alive
	// NOT required if you are publishing once every KEEPALIVE seconds
	if (!mqtt.ping()) {
		mqtt.disconnect();
	}

	// Now we can publish stuff!
	photocell.publish(x);
	Serial.print(F("\nSending photocell val "));
	Serial.print(x);
	Serial.print("...");
	if (!photocell.publish(x++)) {
		Serial.println(F("Failed"));
	} else {
		Serial.println(F("OK!"));
	}

	// Leer el valor del sensor infrarrojo
	int irValue = digitalRead(D0);
	if (irValue == HIGH) {
		Serial.println("Objeto encontrado");
		irsensor.publish("Objeto encontrado");
	}

	// Esperamos 5 segundos entre medidas
	delay(5000);

	// Leemos la humedad relativa
	float h = dht.readHumidity();
	// Leemos la temperatura en grados centígrados (por defecto)
	float t = dht.readTemperature();
	// Leemos la temperatura en grados Fahreheit
	float f = dht.readTemperature(true);

	// Comprobamos si ha habido algún error en la lectura
	if (isnan(h) || isnan(t) || isnan(f)) {
		Serial.println("Error obteniendo los datos del sensor DHT11");
		return;
	}

	// Calcular el índice de calor en Fahreheit
	float hif = dht.computeHeatIndex(f, h);
	// Calcular el índice de calor en grados centígrados
	float hic = dht.computeHeatIndex(t, h, false);

	Serial.print("Humedad: ");
	Serial.print(h);
	Serial.print(" %\t");
	Serial.print("Temperatura: ");
	Serial.print(t);
	Serial.print(" *C ");
	Serial.print(f);
	Serial.print(" *F\t");
	Serial.print("Índice de calor: ");
	Serial.print(hic);
	Serial.print(" *C ");
	Serial.print(hif);
	Serial.println(" *F");

	sensor.publish("Humedad: ");
	sensor.publish(h);
	sensor.publish(" %\t");
	sensor.publish("Temperatura: ");
	sensor.publish(t);
	sensor.publish(" *C ");
	sensor.publish(f);
	sensor.publish(" *F\t");
	sensor.publish("Índice de calor: ");
	sensor.publish(hic);
	sensor.publish(" *C ");
	sensor.publish(hif);
	sensor.publish(" *F");
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
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