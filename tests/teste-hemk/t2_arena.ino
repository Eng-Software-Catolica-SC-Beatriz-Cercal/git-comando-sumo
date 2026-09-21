/*
 * PROJETO ROBÔ DE SUMÔ - base: código do professor (SumoDidatico_Codigo)
 * Por enquanto só anda e não sai da arena. O ataque fica pra depois.
 */

// Sem essa linha o PWM do motor direito (pino 3) trava o receptor IR,
// porque os dois usam o mesmo timer do Arduino.
#define IR_USE_AVR_TIMER1
#include <IRremote.hpp>

// Motor A (Esquerdo)
#define ENA 5
#define IN1 6
#define IN2 7

// Motor B (Direito)
#define ENB 3
#define IN3 2
#define IN4 4

// Sensores
#define SHARP A0
#define SE A1
#define SD A2

// LED RGB
#define LedR 9
#define LedG 10
#define LedB 11

// Receptor IR
#define IR 8

int sensorEsqValue;
int sensorDirValue;

// Ajustar com os valores do T1 e do T2
int line = 700;       // meio entre o valor do preto e o do branco
int velocidade = 150;

bool roboLigado = false;
unsigned long tempoInicio = 0;

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
  IrReceiver.begin(IR, DISABLE_LED_FEEDBACK);
  colorLED(0, 0, 0);
}

void loop() {
  receptorIR();

  if (!roboLigado || millis() - tempoInicio < 5000) {
    return;
  }

  sensorEsqValue = analogRead(SE);
  sensorDirValue = analogRead(SD);

  if (branco(sensorEsqValue) || branco(sensorDirValue)) {
    inWhiteLine();
  }
  else {
    searchMethod();
  }
}

// Se no T1 o branco deu MAIOR que o preto, trocar o < por >
bool branco(int valor) {
  return valor < line;
}

// Dá ré e gira pro lado contrário da borda
void inWhiteLine() {
  bool bordaNaEsquerda = branco(sensorEsqValue);

  motores(-velocidade, -velocidade);
  delay(300);

  if (bordaNaEsquerda) {
    motores(velocidade, -velocidade); // gira pra direita
  }
  else {
    motores(-velocidade, velocidade); // gira pra esquerda
  }
  delay(350);
}

void searchMethod() {
  motores(velocidade, velocidade);
}

// positivo = frente, negativo = trás, de -255 a 255
void motores(int esq, int dir) {
  digitalWrite(IN1, esq > 0);
  digitalWrite(IN2, esq < 0);
  analogWrite(ENA, abs(esq));

  digitalWrite(IN3, dir > 0);
  digitalWrite(IN4, dir < 0);
  analogWrite(ENB, abs(dir));
}

// Funções do professor, sem alteração

void colorLED(int red, int green, int blue) {
  analogWrite(LedR, red);
  analogWrite(LedG, green);
  analogWrite(LedB, blue);
}

void receptorIR()
{
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
    }

    IrReceiver.resume();
  }
}
