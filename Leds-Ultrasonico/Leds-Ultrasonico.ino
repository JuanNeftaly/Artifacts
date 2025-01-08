// trigger y echo
int TRIG = D2;
int ECO = D1;

float t = 0;
long tr = 0;
float l = 0;

float tiempo;
float distancia;

int led = D3;

void setup() {
  // Set pines del ultra
  pinMode(TRIG, OUTPUT);
  pinMode(ECO, INPUT);

  pinMode(led, OUTPUT);

  // serial
  Serial.begin(115200);
}

void loop() {
  // limpiar trigger
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  // encender trigger
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // conversion de datos
  tr = pulseIn(ECO, HIGH);
  t = tr * 0.000001;
  l = (((343.2 * t) * 100) / 2);
  Serial.println(l);

  delay(1000);

  if (l >= 300) {
    Serial.println("El vaso esta vacio");
    digitalWrite(led, HIGH);
  } else if (l >= 120 && l <= 180) {
    Serial.println("El vaso medio lleno");
    digitalWrite(led, LOW);
  } else if (l <= 20) {
    Serial.println("El vaso esta lleno");
    digitalWrite(led, LOW);
  } else {
    Serial.println("El vaso esta en un nivel intermedio");
    digitalWrite(led, LOW);
  }
}
