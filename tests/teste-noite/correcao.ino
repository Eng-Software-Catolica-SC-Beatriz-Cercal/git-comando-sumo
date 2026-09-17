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
 *
 * -------------------------------------------------------------------------------------
 * CORREÇÕES FEITAS NESTA VERSÃO (em relação ao original):
 *
 *   (A) inWhiteLine() usava delay() bloqueante (~700ms). Nesse intervalo o robô não lia
 *       sensores nem escutava o controle IR. Troquei por uma máquina de estados não
 *       bloqueante baseada em millis(), controlada dentro do loop().
 *
 *   (B) colorLED() usava analogWrite() no pino 11 (LedB). O Timer2 do Uno é usado pela
 *       IRremote e também controla o PWM do pino 11, além do pino 3 (ENB) já citado no
 *       comentário original. Ou seja, o mesmo problema dos motores also afeta o canal
 *       azul do LED. Troquei colorLED() para digitalWrite (liga/desliga cada cor, sem
 *       gradiente) para não depender do PWM do Timer2 em nenhum pino.
 *
 *   (C) threshold e line seguem como estavam (500 e 700) - ainda precisam ser calibrados
 *       na arena real com verificaSensores(), isso não dá pra corrigir no código.
 *
 *   (D) A checagem do comando específico do controle em receptorIR() segue comentada de
 *       propósito (pra testar com qualquer controle agora). Deixei um lembrete mais
 *       visível pra não esquecerem de descomentar antes da competição.
 * =====================================================================================
 */

#include <IRremote.hpp> // Certifique-se de ter a biblioteca IRremote instalada

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

// Receptor IR
#define IR 8     // Sensor Infravermelho do Controle


// =====================================================================================
// VARIÁVEIS DE CONFIGURAÇÃO (PODE AJUSTAR CONFORME OS TESTES)
// =====================================================================================

// Leitura dos sensores
int sharpValue;      // Guarda o valor lido pelo sensor de distância
int sensorEsqValue;  // Guarda a cor lida pelo sensor esquerdo
int sensorDirValue;  // Guarda a cor lida pelo sensor direito

// Limiares de Calibração
int threshold = 500; // Valor limite para considerar que detectou um oponente
int line = 700;      // Valor limite para considerar que detectou a linha branca

// Tempos das manobras ao encontrar a borda, em milissegundos
int tempoRe = 300;   // Quanto tempo o robô dá ré antes de girar
int tempoGiro = 400; // Quanto tempo o robô gira para escapar da borda

// Controle do Robô (NÃO ALTERAR)
bool roboLigado = false;       // Estado do robô (Ligado/Desligado via IR)
unsigned long tempoInicio = 0; // Armazena o tempo inicial para a regra dos 5 segundos

// --- (A) Máquina de estados não bloqueante para inWhiteLine() ---
enum EstadoBorda { BORDA_INATIVA, BORDA_RE, BORDA_GIRO };
EstadoBorda estadoBorda = BORDA_INATIVA;
unsigned long tempoEstadoBorda = 0;
bool bordaNaEsquerda = false;


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

  // Configuração do LED RGB
  pinMode(LedR, OUTPUT);
  pinMode(LedG, OUTPUT);
  pinMode(LedB, OUTPUT);

  // Configuração dos Sensores
  pinMode(SHARP, INPUT);
  pinMode(SE, INPUT);
  pinMode(SD, INPUT);

  Serial.begin(9600);

  IrReceiver.begin(IR, DISABLE_LED_FEEDBACK);

  // Configuração da cor do LED RGB -> (Vermelho, Verde, Azul), cada um 0 (apagado) ou 1 (aceso)
  colorLED(0, 0, 0);
}


// =====================================================================================
// LOOP PRINCIPAL (PREENCHA OS ESPAÇOS 'TODO')
// =====================================================================================
void loop()
{
  if (!roboLigado) {
    verificaSensores(); // Mostra os valores dos sensores no Serial Monitor
  }

  // -----------------------------------------------------------------------------------
  // CONTROLE DO ROBÔ E REGRA DOS 5 SEGUNDOS (NÃO ALTERAR NADA NESTE BLOCO)
  // -----------------------------------------------------------------------------------
  receptorIR();

  if (!roboLigado || millis() - tempoInicio < 5000) {
    return; // Aguarda o robô ser ligado e respeita o tempo obrigatório da regra
  }
  // -----------------------------------------------------------------------------------

  // [1] Faça as variáveis dos sensores lerem os pinos analógicos correspondentes
  sharpValue = analogRead(SHARP);
  sensorEsqValue = analogRead(SE);
  sensorDirValue = analogRead(SD);

  // (A) Se já estamos no meio de uma manobra de fuga da borda, continua ela
  // (não bloqueante) e ignora o resto da decisão até terminar.
  if (estadoBorda != BORDA_INATIVA) {
    atualizaInWhiteLine();
    return;
  }

  // [2] Lógica de decisão do robô usando as variáveis de threshold e line
  // A borda vem primeiro: sair da arena perde a luta mesmo com o oponente na frente
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
// ESTRATÉGIAS DO ROBÔ (ÁREA DE PROGRAMAÇÃO)
// =====================================================================================

// (A) Inicia a manobra de fuga da borda: recua e gira para o lado oposto ao
// sensor que viu a borda. Não bloqueia o loop() - só arma o estado inicial.
void inWhiteLine() {
  bordaNaEsquerda = ehBranco(sensorEsqValue);

  motores(-1, -1); // ré
  estadoBorda = BORDA_RE;
  tempoEstadoBorda = millis();
}

// (A) Chamada a cada volta do loop() enquanto a manobra de borda está em andamento.
// Troca de estado (ré -> giro -> inativa) sem usar delay().
void atualizaInWhiteLine() {
  unsigned long agora = millis();

  if (estadoBorda == BORDA_RE && agora - tempoEstadoBorda >= (unsigned long)tempoRe) {
    if (bordaNaEsquerda) {
      motores(1, -1); // gira para a direita
    }
    else {
      motores(-1, 1); // gira para a esquerda
    }
    estadoBorda = BORDA_GIRO;
    tempoEstadoBorda = agora;
  }
  else if (estadoBorda == BORDA_GIRO && agora - tempoEstadoBorda >= (unsigned long)tempoGiro) {
    estadoBorda = BORDA_INATIVA; // manobra concluída, volta pra decisão normal
  }
}

// Vai para frente enquanto o Sharp estiver vendo o oponente
void moveToOponent() {
  motores(1, 1);
}

// Gira no lugar até o Sharp encontrar o oponente
void searchMethod() {
  motores(1, -1);
}


// =====================================================================================
// FUNÇÕES AUXILIARES DAS ESTRATÉGIAS
// =====================================================================================

// Sentido de cada motor: 1 = frente, -1 = ré, 0 = parado
void motores(int esquerdo, int direito) {
  // ENA/ENB sempre em HIGH (velocidade máxima): a IRremote usa o Timer2 do Uno,
  // o mesmo do PWM do pino 3 (ENB), então analogWrite(ENB, ...) não é confiável
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, esquerdo == 1 ? HIGH : LOW);
  digitalWrite(IN2, esquerdo == -1 ? HIGH : LOW);

  digitalWrite(IN3, direito == 1 ? HIGH : LOW);
  digitalWrite(IN4, direito == -1 ? HIGH : LOW);
}

// Se na calibração o branco der valor MAIOR que o preto, trocar '<' por '>'
bool ehBranco(int valor) {
  return valor < line;
}


// =====================================================================================
//