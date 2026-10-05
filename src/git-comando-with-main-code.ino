// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// PERSEGUIDOR_GIT_COMANDO.ino
// Código para o robô perseguidor do desafio de robótica da GIT
// Autor: Equipe Git Comando
// Data: 2024-06-10
// Ultima Atualização: 2024-06-10 por Miguel Rocha Xavier
// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

/*
 * =====================================================================================
 *                         PROJETO ROBÔ DE SUMÔ - ARDUINO
 * =====================================================================================
 *
 * GUIA PARA OS ALUNOS:
 *
 * [1] O QUE VOCÊ DEVE ALTERAR:
 *     - Ajustar as variáveis 'threshold' e 'line' conforme os testes na arena.
 *     - Preencher as leituras dos sensores no loop().
 *     - Preencher a lógica das condições no loop() (if / else if).
 *     - Criar as estratégias dentro das funções:
 *       -> inWhiteLine()   : O que fazer ao detectar a borda da arena.
 *       -> moveToOponent() : O que fazer ao detectar o adversário.
 *       -> searchMethod()  : Como procurar o adversário na arena.
 *
 * [2] O QUE VOCÊ NÃO DEVE ALTERAR (Risco de quebrar o robô):
 *     - Definição dos Pinos (#define) - A menos que a fiação seja alterada fisicamente.
 *     - A função setup() e as funções do sistema (colorLED e receptorIR).
 *     - O controle do temporizador de 5 segundos no inicio do loop().
 * =====================================================================================
 */

// Atualmente sem a biblioteca do IRremote para testes
//#include <IRremote.hpp> // Certifique-se de ter a biblioteca IRremote instalada

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


// =====================================================================================
// VARIÁVEIS DE CONFIGURAÇÃO (PODE AJUSTAR CONFORME OS TESTES)
// =====================================================================================

// Leitura dos sensores
int sharpValue;      // Guarda o valor lido pelo sensor de distância
int sensorEsqValue;  // Guarda a cor lida pelo sensor esquerdo
int sensorDirValue;  // Guarda a cor lida pelo sensor direito

// Limiares de Calibração
int threshold = 400; // Valor limite para considerar que detectou um oponente (acima disso = oponente perto -> AJUSTE pelo Serial)
int line = 200;      // Valor limite para considerar que detectou a linha branca (abaixo disso = branco/borda)

// Ajustes - linha
int velocidade     = 140;   // velocidade andando
int velocidadeGiro = 180;   // velocidade no giro de fuga
int tempoFreio     = 150;
int tempoRe        = 450;
int tempoReLado    = 400;
int tempoGiro180   = 250;
int tempoGiroLado  = 220;

// Ajustes - oponente
int velocidadeAtaque = 255;  // força máxima
int velocidadeBusca  = 160;  // giro de busca (devagar para o Sharp conseguir ver); 160 parece ser o ideal

// Ajustes - largada
int tempoArrancada = 1000;    // ms andando pra frente na largada


// =====================================================================================
// SETUP DO SISTEMA (NÃO ALTERAR)
// =====================================================================================
void setup() {
  // Configuração dos Pinos dos Motores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Configuração dos Sensores
  pinMode(SHARP, INPUT);
  pinMode(SE, INPUT);
  pinMode(SD, INPUT);

  Serial.begin(9600);

  // -----------------------------------------------------------------------------------
  // REGRA DOS 5 SEGUNDOS (NÃO ALTERAR NADA NESTE BLOCO)
  // -----------------------------------------------------------------------------------
  delay(5000); // Respeita o tempo obrigatório da regra
  // -----------------------------------------------------------------------------------

  // Avança um pouco antes de começar a lutar (parando se encontrar a borda)
  unsigned long inicio = millis();
  forward(velocidade);
  while (millis() - inicio < tempoArrancada) {
    if (analogRead(SE) < line || analogRead(SD) < line) {
      break;  // viu branco, deixa o loop() tratar
    }
  }
}


// =====================================================================================
// LOOP PRINCIPAL
// =====================================================================================
void loop() {
  // [1] Faça as variáveis dos sensores lerem os pinos analógicos correspondentes
  sensorEsqValue = analogRead(SE);
  sensorDirValue = analogRead(SD);
  sharpValue     = analogRead(SHARP);

  // Serial.println(sharpValue);  // descomente para calibrar o threshold

  bool brancoEsq = sensorEsqValue < line;
  bool brancoDir = sensorDirValue < line;

  // [2] Monte a lógica de decisão do robô usando as variáveis de threshold e line
  // ---------- 1) LINHA BRANCA (prioridade máxima) ----------
  if (brancoEsq || brancoDir) { // Condição para detectar a linha branca na arena
    inWhiteLine();
  }
  // ---------- 2) OPONENTE PERTO -> ATAQUE ----------
  else if (sharpValue > threshold) { // Condição para detectar o oponente
    moveToOponent();
  }
  // ---------- 3) NADA -> BUSCA GIRANDO ----------
  else {
    searchMethod();
  }
}


// =====================================================================================
// ESTRATÉGIAS DO ROBÔ (ÁREA DE PROGRAMAÇÃO)
// =====================================================================================

// Defina os movimentos do robô ao encontrar a linha branca
void inWhiteLine() {
  // Usa as leituras feitas no loop() para saber qual lado viu a borda
  bool brancoEsq = sensorEsqValue < line;
  bool brancoDir = sensorDirValue < line;

  brake();
  delay(tempoFreio);

  sensorEsqValue = analogRead(SE);
  sensorDirValue = analogRead(SD);
  brancoEsq = brancoEsq || (sensorEsqValue < line);
  brancoDir = brancoDir || (sensorDirValue < line);

  if (brancoEsq && brancoDir) {
    backward();
    delay(tempoRe);
    turnRight(velocidadeGiro);
    delay(tempoGiro180);
  }
  else if (brancoEsq) {
    backward();
    delay(tempoReLado);
    turnRight(velocidadeGiro);
    delay(tempoGiroLado);
  }
  else {
    backward();
    delay(tempoReLado);
    turnLeft(velocidadeGiro);
    delay(tempoGiroLado);
  }
}

// Defina os movimentos do robô ao detectar o adversário
void moveToOponent() {
  forward(velocidadeAtaque);
}

// Defina o padrão de busca do robô enquanto não encontra nada
void searchMethod() {
  turnRight(velocidadeBusca);
}


// =====================================================================================
// FUNÇÕES DE SUPORTE DO SISTEMA (NÃO ALTERAR)
// =====================================================================================

// ---------------- MOVIMENTOS ----------------

void forward(int vel) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, vel);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, vel);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, velocidade);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, velocidade);
}

// Gira no eixo para a direita
void turnRight(int vel) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, vel);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, vel);
}

// Gira no eixo para a esquerda
void turnLeft(int vel) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, vel);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, vel);
}

// Freio ativo
void brake() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 255);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 255);
}

// Parada dos motores
void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
