// ============================================================
//  Seguidor de línea + Evasión programada
//  Arduino Nano + L298N + HC-SR04 + 2x TCRT5000
// ============================================================

// ── Pines ultrasónico ────────────────────────────────────────
#define TRIG_PIN 2
#define ECHO_PIN 3

// ── Pines L298N ──────────────────────────────────────────────
#define ENA 5
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10
#define ENB 6

// ── Pines sensores ───────────────────────────────────────────
#define SENSOR_IZQ 11
#define SENSOR_DER 12

// ── Parámetros ───────────────────────────────────────────────
int velocidad = 190; // 70 para 12v
int distancia_limite = 20;

bool modoEvasion = false;

// ============================================================

void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(SENSOR_IZQ, INPUT);
  pinMode(SENSOR_DER, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);
}

// ============================================================

void loop() {

  int distancia = medirDistancia();

  if (distancia <= distancia_limite && !modoEvasion) {
    modoEvasion = true;
    evadirObstaculo();
  }

  if (!modoEvasion) {
    seguirLinea();
  }
}

// ============================================================
// MEDIR DISTANCIA
// ============================================================

int medirDistancia() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracion = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duracion == 0) return 999;

  int distancia = duracion * 0.034 / 2;

  if (distancia <= 0 || distancia > 400) return 999;

  return distancia;
}

// ============================================================
// SEGUIR LINEA NORMAL
// ============================================================

void seguirLinea() {

  int izq = digitalRead(SENSOR_IZQ);
  int der = digitalRead(SENSOR_DER);

  if (izq == LOW && der == LOW) {
    avanzar();
  }
  else if (izq == LOW && der == HIGH) {
    girarIzquierda();
  }
  else if (izq == HIGH && der == LOW) {
    girarDerecha();
  }
  else {
    detener();
  }
}

// ============================================================
// EVASION PROGRAMADA
// ============================================================

void evadirObstaculo() {

  Serial.println("OBSTACULO DETECTADO");

  detener();
  delay(150);

  girarDerecha();
  delay(1200);

  detener();
  delay(150);

  avanzar();
  delay(700);
  
  detener();
  delay(150);

  girarIzquierda();
  delay(1200);

  detener();
  delay(150);

  avanzar();
  delay(1000);

  detener();
  delay(150);

  girarIzquierda();
  delay(1200);

  avanzar();

  while (true) {

    int izq = digitalRead(SENSOR_IZQ);
    int der = digitalRead(SENSOR_DER);

    if (izq == LOW || der == LOW) {
      break;   // Línea encontrada
    }

    girarIzquierda();
  }

  girarDerecha();
  delay(200);

  modoEvasion = false;

  Serial.println("LINEA REENCONTRADA");
}

// ============================================================
// MOVIMIENTOS
// ============================================================

void avanzar() {

  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void girarIzquierda() {

  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void girarDerecha() {

  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void detener() {

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}