// Versão semelhante ao teste sem variável, só com valores numéricos
// Esse arquivo serve como base para testar a lógica do robô com números fixos.
// A ideia é manter a lógica simples e fácil de ajustar durante os testes.

#include <IRremote.hpp>

// Pinos dos motores
#define ENA 5
#define IN1 6
#define IN2 7
#define ENB 3
#define IN3 2
#define IN4 4

// Pinos dos sensores e LED
#define SHARP A0
#define SE A1
#define SD A2
#define LedR 9
#define LedG 10
#define LedB 11
#define IR 8

// Leitura dos sensores
int sharpValue;      // valor do sensor de distância
int sensorEsqValue;  // valor do sensor de linha da esquerda
int sensorDirValue;  // valor do sensor de linha da direita

// Tempo das manobras para recuperar da borda
int tempoRe = 300;      // tempo de ré
int tempoGiro = 400;    // tempo do giro
int estadoBorda = 0;    // 0 = inativo, 1 = ré, 2 = giro
unsigned long tempoEstadoBorda = 0;
bool bordaNaEsquerda = false;

// Ajustes de velocidade/força
const float fatorVelocidade = 0.9; // reduz a velocidade geral
const float forcaBusca = 0.7;        // força de rotação em busca do oponente
const float forcaAtaque = 1.0;       // força maior para empurrar o oponente

bool roboLigado = false;
unsigned long tempoInicio = 0;

void setup() {
  // Configura os pinos dos motores como saída
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Configura os pinos do LED RGB
  pinMode(LedR, OUTPUT);
  pinMode(LedG, OUTPUT);
  pinMode(LedB, OUTPUT);

  // Configura os sensores como entrada
  pinMode(SHARP, INPUT);
  pinMode(SE, INPUT);
  pinMode(SD, INPUT);

  // Inicia comunicação serial para debug
  Serial.begin(9600);
  IrReceiver.begin(IR, DISABLE_LED_FEEDBACK);
  colorLED(0, 0, 0);
}

void loop() {
  if (!roboLigado) {
    verificaSensores();
  }

  receptorIR();

  if (!roboLigado || millis() - tempoInicio < 5000) {
    return;
  }

  // Lê os sensores em cada ciclo
  sharpValue = analogRead(SHARP);
  sensorEsqValue = analogRead(SE);
  sensorDirValue = analogRead(SD);

  // Se o robô já entrou em manobra de borda, ele não pode continuar normal
  if (estadoBorda != 0) {
    atualizaInWhiteLine();
    return;
  }

  // 1. Detecta borda da arena
  if (sensorEsqValue < 700 || sensorDirValue < 700) {
    inWhiteLine();
  }
  // 2. Detecta oponente
  else if (sharpValue > 500) {
    moveToOponent();
  }
  // 3. Se não viu nada, procura
  else {
    searchMethod();
  }
}

// Ao detectar a borda, o robô dá ré para afastar-se da linha
void inWhiteLine() {
  bordaNaEsquerda = sensorEsqValue < 700;
  motores(-1.0, -1.0);
  estadoBorda = 1;
  tempoEstadoBorda = millis();
}

// Controla o estado de borda: ré e depois giro para sair da linha
void atualizaInWhiteLine() {
  unsigned long agora = millis();

  if (estadoBorda == 1 && agora - tempoEstadoBorda >= (unsigned long)tempoRe) {
    if (bordaNaEsquerda) {
      motores(1.0, -1.0);
    } else {
      motores(-1.0, 1.0);
    }
    estadoBorda = 2;
    tempoEstadoBorda = agora;
  }
  else if (estadoBorda == 2 && agora - tempoEstadoBorda >= (unsigned long)tempoGiro) {
    estadoBorda = 0;
  }
}

// Quando acha o oponente, acelera para empurrar com mais força
void moveToOponent() {
  motores(forcaAtaque, forcaAtaque);
}

// Quando não encontra oponente, faz um giro de busca
void searchMethod() {
  motores(forcaBusca, -forcaBusca);
}

// Função central para mover os motores com velocidade proporcional
void motores(float esquerdo, float direito) {
  // Converte os valores de -1 a 1 em PWM de 0 a 255
  int velEsq = (int)(constrain(abs(esquerdo), 0.0, 1.0) * 255.0 * fatorVelocidade);
  int velDir = (int)(constrain(abs(direito), 0.0, 1.0) * 255.0 * fatorVelocidade);

  // Motor esquerdo
  if (esquerdo >= 0.0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, velEsq);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, velEsq);
  }

  // Motor direito
  if (direito >= 0.0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, velDir);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, velDir);
  }
}

// Controla a cor do LED RGB
void colorLED(int red, int green, int blue) {
  digitalWrite(LedR, red ? HIGH : LOW);
  digitalWrite(LedG, green ? HIGH : LOW);
  digitalWrite(LedB, blue ? HIGH : LOW);
}

// Liga ou desliga o robô pelo controle infravermelho
void receptorIR() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return;
    }

    roboLigado = !roboLigado;

    if (roboLigado) {
      tempoInicio = millis();
    }
    else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);
    }

    IrReceiver.resume();
  }
}

// Mostra os valores lidos pelos sensores no monitor serial
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
