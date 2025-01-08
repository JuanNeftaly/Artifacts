#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// DHT
#include <DHT.h>
#define DHTPIN D2
#define DHTTYPE DHT11

// init DHT11
DHT dht(DHTPIN, DHTTYPE);

//********DELAY PARA MOSTRAR ESTADO DEL WIFI*********
unsigned long interval = 30000;	 // delay en ms
unsigned long previousMillis = 0;

WiFiClient wlanclient;
PubSubClient mqttClient(wlanclient);

//***************CONFIGURACIÓN DE RED****************
const char *ssid = "";	  // nombre de la red a utilizar
const char *passwd = "";  // contra de la red

//***************CONFIGURACIÓN DE MQTT***************
char *server = "";  // colocar la ip de la red a la q estamos conectados
int port = 1883;

// usuario y contra del mqtt
const char *mqtt_user = "";
const char *mqtt_password = "";

// Definir todos los "topics" a los cuales se desea suscribir
const char *topics[] = {"/test/message", "/test/Humedad", "/test/TempC",
			"/test/TempF", "/test/LDR"};

String LedMessage = "0";
//***************CALLBACK DE RESOLUCIÓN A SUBSCRIBES***************
void mqttCallback(char *topicChar, byte *payload, unsigned int length) {
	Serial.println();

	//*********Obtener el topic convertido a tipo String*************
	String topic = String(topicChar);
	Serial.print("Message arrived on Topic: ");
	Serial.println(topic);

	//*********Obtener el mensaje convertido a tipo String***********
	String message = "";

	for (unsigned int i = 0; i < length; i++) {
		message += (char)payload[i];  // Añadir cada carácter al String
	}

	//**********Definir manejo de mensajes entrantes por topic*******
	// Ej:
	if (topic == "/test/message") {
		Serial.println(message);
		// si recibimos cierto mensaje podemos hacer algo
	}

	// led logic
	if (topic == "/test/Led") {
		Serial.println(LedMessage);
		// si recibimos cierto mensaje podemos hacer algo
		LedMessage = message;
	}
}

//***************FUNCIONES PARA PUBLICAR Y SUSCRIBIR***************
boolean publishToTopic(char *topic, char *message) {
	return mqttClient.publish(topic, message);
}

void subscribeToTopics() {
	const int numTopics = sizeof(topics) / sizeof(topics[0]);

	for (int i = 0; i < numTopics; i++) {
		mqttClient.subscribe(topics[i]);
	}
}

//***************DEFINICIÓN DE VARIABLES GLOBALES******************
// Ej. int LED = D2;
int LDR = A0;
int value = 0;
int led1 = D1;
int led2 = D3;
int led3 = D4;

void setup() {
	//*********Sección para inicializar sensores
	// Ej. pinMode(LED, OUTPUT);
	dht.begin();

	//*********Inicializar monitor serial***********
	Serial.begin(115200);

	//*********Configurar conexión wifi*************
	WiFi.begin(ssid, passwd);

	Serial.print("Connecting to AP");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(100);
	}
	Serial.println();
	Serial.print("Connected to WiFi Access Point, Got an IP address: ");
	Serial.println(WiFi.localIP());
	// Configurar la reconexión automática ante perdida de señal
	WiFi.setAutoReconnect(true);
	WiFi.persistent(
	    true);  // Configurar la reconexión automática ante perdida de señal

	//*********Configurar conexión MQTT*************
	mqttClient.setServer(server, port);
	mqttClient.setCallback(mqttCallback);

	// Realizar conexión al MQTT Broker
	if (mqttClient.connect("ESP-Client", mqtt_user, mqtt_password)) {
		Serial.println("Connected to MQTT Broker");
	} else {
		Serial.println("MQTT Broker connection failed");
		Serial.println(mqttClient.state());
		delay(200);
	}

	// suscribirse a los temas.
	subscribeToTopics();
}

void loop() {
	// Imprime la señal de la red cada <interval> segundos
	printWifiStatus();

	// verificar si no hay conexión con el broker, si es así reconectarse:
	if (!mqttClient.connected()) {
		reconnect();
	}

	// Loop de funcionamiento de la libreria PubSubClient
	mqttClient.loop();

	//********Tú código de sensores, funcionamiento, etc. Va aquí********
	// Leemos la humedad relativa
	float humedad = dht.readHumidity();
	// Leemos la temperatura en grados centígrados (por defecto)
	float tempC = dht.readTemperature();
	// Leemos la temperatura en grados Fahreheit
	float tempF = dht.readTemperature(true);

	// Comprobamos si ha habido algún error en la lectura
	if (isnan(humedad) || isnan(tempC) || isnan(tempF)) {
		Serial.println("Error obteniendo los datos del sensor DHT11");
		return;
	}

	// imprimir DHT
	printDHT(humedad, tempC, tempF);

	// Send data
	char *floatHumedad = floatToChar(
	    humedad, 2);  // El segundo parámetro es la cantidad de decimales
	publishToTopic("/test/Humedad", floatHumedad);
	free(floatHumedad);

	char *floatTempC = floatToChar(
	    tempC, 2);	// El segundo parámetro es la cantidad de decimales
	publishToTopic("/test/TempC", floatTempC);
	free(floatTempC);

	char *floatTempF = floatToChar(
	    tempF, 2);	// El segundo parámetro es la cantidad de decimales
	publishToTopic("/test/TempF", floatTempF);
	free(floatTempF);

	value = analogRead(LDR);
	Serial.print(value);

	char *intLDR = intToChar(
	    value);  // El segundo parámetro es la cantidad de decimales
	publishToTopic("/test/LDR", intLDR);
	free(intLDR);

	delay(2000);
}

void printDHT(float h, float t, float f) {
	// Calcula el índice de calor (heat index)
	float hif =
	    dht.computeHeatIndex(f, h);	 // Índice de calor en Fahrenheit
	float hic =
	    dht.computeHeatIndex(t, h, false);	// Índice de calor en Celsius

	// Imprime los valores
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
}

void reconnect() {
	while (!mqttClient.connected()) {
		Serial.println();
		Serial.println("Trying to connect to the MQTT broker...");

		if (mqttClient.connect("ESP-Client", mqtt_user,
				       mqtt_password)) {
			Serial.println("Connected to MQTT Broker");

			// suscribirse nuevamente a los temas si la conexión
			// regresa.
			subscribeToTopics();
		} else {
			Serial.print("Fallo, rc=");
			Serial.println(mqttClient.state());
			Serial.println("Trying to connect each 5 seconds");
			// Esperar 5 segundos antes de reintentar
			delay(5000);
		}
		printWifiStatus();
	}
}

void printWifiStatus() {
	// Imprime la señal de la red cada <interval> segundos
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= interval) {
		Serial.println("");
		Serial.println("----------------------------------------");
		switch (WiFi.status()) {
			case WL_NO_SSID_AVAIL:
				Serial.println(
				    "Wifi Configured SSID cannot be reached");
				break;
			case WL_CONNECTED:
				Serial.println(
				    "Connection Wifi successfully established");
				break;
			case WL_CONNECT_FAILED:
				Serial.println("Wifi Connection failed");
				break;
		}
		Serial.printf("Connection status: %d\n", WiFi.status());
		Serial.print("RRSI: ");
		Serial.println(WiFi.RSSI());
		Serial.println("----------------------------------------");
		previousMillis = currentMillis;
	}
}

//***************FUNCIONES AUXILIARES DE CONVERSIÓN***************

//***************float -> *char********************
char *floatToChar(float number, int precision) {
	// Calcular el tamaño necesario (incluyendo el signo y el punto decimal)
	int length = snprintf(NULL, 0, "%.*f", precision, number);

	// Asignar memoria para la cadena resultante
	char *result =
	    (char *)malloc(length + 1);	 // +1 para el terminador nulo

	// Verificar si la asignación fue exitosa
	if (result != NULL) {
		snprintf(result, length + 1, "%.*f", precision, number);
	}

	return result;
}

//***************int -> *char********************
char *intToChar(int number) {
	// Calcular el tamaño necesario (incluyendo el signo)
	int length = snprintf(NULL, 0, "%d", number);

	// Asignar memoria para la cadena resultante
	char *result =
	    (char *)malloc(length + 1);	 // +1 para el terminador nulo

	// Verificar si la asignación fue exitosa
	if (result != NULL) {
		snprintf(result, length + 1, "%d", number);
	}

	return result;
}