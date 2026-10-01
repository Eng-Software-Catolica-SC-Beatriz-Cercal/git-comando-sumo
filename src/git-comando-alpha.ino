// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// PERSEGUIDOR_GIT_COMANDO.ino
// Código para o robô perseguidor do desafio de robótica da GIT
// Autor: Equipe Git Comando
// Data: 2024-06-10
// Ultima Atualização: 2024-06-10 por Miguel Rocha Xavier
// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Atualmente sem a biblioteca do IRremote para testes
//#include <IRremote.hpp>

// Motor A (Esquerdo)
#define ENA 5
#define IN1 6
#define IN2 7

// Motor B (Direito)
#define ENB 3
#define IN3 2
#define IN4 4

// Sensores
#define SHARP A0  // Proximidade
#define SE A1     // Linha esquerdo
#define SD A2     // Linha direito

// Ajustes - linha
int limiteBranco   = 200;   // abaixo disso = branco (borda)
int velocidade     = 140;   // velocidade andando
int velocidadeGiro = 180;   // velocidade no giro de fuga
int tempoFreio     = 150;
int tempoRe        = 450;
int tempoReLado    = 400;
int tempoGiro180   = 250;
int tempoGiroLado  = 220;

// Ajustes - oponente
int limiteOponente   = 500;  // acima disso = oponente perto -> AJUSTE pelo Serial
int velocidadeAtaque = 255;  // força máxima
int velocidadeBusca  = 160;  // giro de busca (devagar para o Sharp conseguir ver); 160 parece ser o ideal

// Ajustes - largada
int tempoArrancada = 1000;    // ms andando pra frente na largada

int valorEsq;
int valorDir;
int sharpValue;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(SE, INPUT);
  pinMode(SD, INPUT);
  pinMode(SHARP, INPUT);

  Serial.begin(9600);

  // Regra dos 5 segundos
  delay(5000);

  // Avança um pouco antes de começar a lutar (parando se encontrar a borda)
  unsigned long inicio = millis();
  frente(velocidade);
  while (millis() - inicio < tempoArrancada) {
    if (analogRead(SE) < limiteBranco || analogRead(SD) < limiteBranco) {
      break;  // viu branco, deixa o loop() tratar
    }
  }
}

void loop() {
  valorEsq   = analogRead(SE);
  valorDir   = analogRead(SD);
  sharpValue = analogRead(SHARP);

  // Serial.println(sharpValue);  // descomente para calibrar o limiteOponente

  bool brancoEsq = valorEsq < limiteBranco;
  bool brancoDir = valorDir < limiteBranco;

  // ---------- 1) LINHA BRANCA (prioridade máxima) ----------
  if (brancoEsq || brancoDir) {
    freiar();
    delay(tempoFreio);

    valorEsq = analogRead(SE);
    valorDir = analogRead(SD);
    brancoEsq = brancoEsq || (valorEsq < limiteBranco);
    brancoDir = brancoDir || (valorDir < limiteBranco);

    if (brancoEsq && brancoDir) {
      re();
      delay(tempoRe);
      girarDireita(velocidadeGiro);
      delay(tempoGiro180);
    }
    else if (brancoEsq) {
      re();
      delay(tempoReLado);
      girarDireita(velocidadeGiro);
      delay(tempoGiroLado);
    }
    else {
      re();
      delay(tempoReLado);
      girarEsquerda(velocidadeGiro);
      delay(tempoGiroLado);
    }
  }
  // ---------- 2) OPONENTE PERTO -> ATAQUE ----------
  else if (sharpValue > limiteOponente) {
    frente(velocidadeAtaque);
  }
  // ---------- 3) NADA -> BUSCA GIRANDO ----------
  else {
    girarDireita(velocidadeBusca);
  }
}

// ---------------- MOVIMENTOS ----------------

void frente(int vel) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, vel);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, vel);
}

void re() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, velocidade);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, velocidade);
}

// Gira no eixo para a direita
void girarDireita(int vel) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, vel);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, vel);
}

// Gira no eixo para a esquerda
void girarEsquerda(int vel) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, vel);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, vel);
}

// Freio ativo
void freiar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 255);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 255);
}

void parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
