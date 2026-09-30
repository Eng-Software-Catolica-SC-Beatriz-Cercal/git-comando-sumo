// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// PERSEGUIDOR_GIT_COMANDO.ino
// Código para o robô perseguidor do desafio de robótica da GIT
// Autor: Miguel Rocha Xavier
// Data: 2024-06-10
// Ultima Atualização: 2024-06-10 por Miguel Rocha Xavier
// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

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
#define SHARP A0 // Sensor de Proximidade
#define SE A1    // Sensor de Linha (Esquerdo)
#define SD A2    // Sensor de Linha (Direito)

// LED RGB
#define LedR 9
#define LedG 10
#define LedB 11

// Receptor IR
#define IR 8

// Estados
#define NORMAL 0
#define RE 1
#define GIRO 2

int threshold = 150;  // Sharp acima disso = adversário na frente
int line = 700;       // sensor de linha abaixo disso = borda branca

bool roboLigado = false;
unsigned long tempoInicio = 0;

int estado = NORMAL;
unsigned long tempoEstado = 0;
int lado = 1;                     // 1 = gira para a direita, -1 = para a esquerda
unsigned long ultimaVezViu = 0;   // última vez que o Sharp viu o adversário
unsigned long inicioBusca = 0;    // quando começou a procurar

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

  unsigned long agora = millis();
  int sharp = analogRead(SHARP);
  int esq = analogRead(SE);
  int dir = analogRead(SD);

  // Viu a borda: começa a fuga (ré + giro para o lado oposto)
  if (estado == NORMAL && (esq < line || dir < line)) {
    estado = RE;
    tempoEstado = agora;
    if (esq < line) {
      lado = 1;   // borda na esquerda -> gira para a direita
    } else {
      lado = -1;  // borda na direita -> gira para a esquerda
    }
  }

  // Ré por 250 ms
  if (estado == RE) {
    colorLED(255, 255, 255);
    motores(-255, -255);
    if (agora - tempoEstado > 250) {
      estado = GIRO;
      tempoEstado = agora;
    }
    return;
  }

  // Gira por 300 ms para longe da borda
  if (estado == GIRO) {
    colorLED(255, 255, 255);
    motores(204 * lado, -204 * lado);
    if (agora - tempoEstado > 300) {
      estado = NORMAL;
      inicioBusca = agora;
    }
    return;
  }

  // Perseguição
  if (sharp > threshold) {
    // Adversário na frente: ataque total
    ultimaVezViu = agora;
    colorLED(255, 0, 0);
    motores(255, 255);
  } else if (agora - ultimaVezViu < 300) {
    // Acabou de perder o alvo: continua reto um pouco
    colorLED(255, 80, 0);
    motores(217, 217);
    inicioBusca = agora;
  } else if (agora - inicioBusca < 2000) {
    // Procurando: gira no lugar
    colorLED(0, 0, 255);
    motores(150 * lado, -150 * lado);
  } else if (agora - inicioBusca < 2500) {
    // Não achou ninguém: anda para frente para mudar de posição
    colorLED(0, 255, 0);
    motores(200, 200);
  } else {
    // Recomeça a busca girando para o outro lado
    inicioBusca = agora;
    lado = -lado;
  }
}

// =====================================================================================
// FUNÇÕES DE SUPORTE
// =====================================================================================

// Controla os dois motores: valores de -255 (ré total) até 255 (frente total)
void motores(int esquerdo, int direito) {
  if (esquerdo >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    esquerdo = -esquerdo;
  }
  analogWrite(ENA, constrain(esquerdo, 0, 255));

  if (direito >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    direito = -direito;
  }
  analogWrite(ENB, constrain(direito, 0, 255));
}

void parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void colorLED(int red, int green, int blue) {
  analogWrite(LedR, red);
  analogWrite(LedG, green);
  analogWrite(LedB, blue);
}

void receptorIR() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return;
    }
    roboLigado = !roboLigado;
    if (roboLigado) {
      tempoInicio = millis();
      estado = NORMAL;
      inicioBusca = millis() + 5000;
      colorLED(255, 255, 0); // amarelo = contagem dos 5 segundos
    } else {
      parar();
      colorLED(0, 0, 0);
    }
    IrReceiver.resume();
  }
}
