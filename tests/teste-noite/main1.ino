// =====================================================================================
// DEFINIÇÃO DOS PINOS (NÃO ALTERA)
// =====================================================================================

// Motor A (Esquerdo)
#define ENA 5
#define IN1 6
#define IN2 7

// Motor B (Direito)
#define ENB 3
#define IN3 2
#define IN4 4

// Sensores
#define SHARP A0 // Sensor de Proximidade
#define SE A1    // Sensor de Linha (Esquerdo)
#define SD A2    // Sensor de Linha (Direito)

// LED RGB
#define LedR 9   // Vermelho
#define LedG 10  // Verde
#define LedB 11  // Azul

// =====================================================================================
// VARIÁVEIS DE CONFIGURAÇÃO (PODE AJUSTAR CONFORME OS TESTES)
// =====================================================================================

// Leitura dos sensores
int sharpValue;
int sensorEsqValue;
int sensorDirValue;

// Limiares de calibração
int threshold = 500;
int line = 700;

// Tempos das manobras ao encontrar a borda, em milissegundos
int tempoRe = 300;
int tempoGiro = 400;

// Máquina de estados para evitar delay() bloqueante
enum EstadoBorda { BORDA_INATIVA, BORDA_RE, BORDA_GIRO };
EstadoBorda estadoBorda = BORDA_INATIVA;
unsigned long tempoEstadoBorda = 0;
bool bordaNaEsquerda = false;

// =====================================================================================
// SETUP DO SISTEMA (NÃO ALTERA)
// =====================================================================================
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(LedR, OUTPUT);
  pinMode(LedG, OUTPUT);
  pinMode(LedB, OUTPUT);

  pinMode(SHARP, INPUT);
  pinMode(SE, INPUT);
  pinMode(SD, INPUT);

  Serial.begin(9600);

  colorLED(0, 0, 0);
}

// =====================================================================================
// LOOP PRINCIPAL
// =====================================================================================
void loop() {
  sharpValue = analogRead(SHARP);
  sensorEsqValue = analogRead(SE);
  sensorDirValue = analogRead(SD);

  if (estadoBorda != BORDA_INATIVA) {
    atualizaInWhiteLine();
    return;
  }

  if (ehBranco(sensorEsqValue) || ehBranco(sensorDirValue)) {
    inWhiteLine();
  }
  else if (sharpValue > threshold) {
    moveToOponent();
  }
  else {
    searchMethod();
  }
}

// =====================================================================================
// ESTRATÉGIAS DO ROBÔ
// =====================================================================================

void inWhiteLine() {
  bordaNaEsquerda = ehBranco(sensorEsqValue);
  motores(-1, -1);
  estadoBorda = BORDA_RE;
  tempoEstadoBorda = millis();
}

void atualizaInWhiteLine() {
  unsigned long agora = millis();

  if (estadoBorda == BORDA_RE && agora - tempoEstadoBorda >= (unsigned long)tempoRe) {
    if (bordaNaEsquerda) {
      motores(1, -1);
    }
    else {
      motores(-1, 1);
    }
    estadoBorda = BORDA_GIRO;
    tempoEstadoBorda = agora;
  }
  else if (estadoBorda == BORDA_GIRO && agora - tempoEstadoBorda >= (unsigned long)tempoGiro) {
    estadoBorda = BORDA_INATIVA;
  }
}

void moveToOponent() {
  motores(1, 1);
}

void searchMethod() {
  motores(1, -1);
}

// =====================================================================================
// FUNÇÕES AUXILIARES DAS ESTRATÉGIAS
// =====================================================================================

void motores(int esquerdo, int direito) {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, esquerdo == 1 ? HIGH : LOW);
  digitalWrite(IN2, esquerdo == -1 ? HIGH : LOW);

  digitalWrite(IN3, direito == 1 ? HIGH : LOW);
  digitalWrite(IN4, direito == -1 ? HIGH : LOW);
}

bool ehBranco(int valor) {
  return valor < line;
}

// =====================================================================================
// FUNÇÕES DE SUPORTE DO SISTEMA
// =====================================================================================

void colorLED(int red, int green, int blue) {
  digitalWrite(LedR, red ? HIGH : LOW);
  digitalWrite(LedG, green ? HIGH : LOW);
  digitalWrite(LedB, blue ? HIGH : LOW);
}

void verificaSensores() {
  delay(500);
  sharpValue = analogRead(SHARP);
  sensorEsqValue = analogRead(SE);
  sensorDirValue = analogRead(SD);

  Serial.print("Sensor de Proximidade (SHARP): ");
  Serial.println(sharpValue);
  Serial.print("Sensor de Linha Esquerdo (SE): ");
  Serial.println(sensorEsqValue);
  Serial.print("Sensor de Linha Direito (SD): ");
  Serial.println(sensorDirValue);
}