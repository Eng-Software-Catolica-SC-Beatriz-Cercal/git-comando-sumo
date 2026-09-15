///teste
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

  // Configuração da cor do LED RGB -> (Vermelho, Verde, Azul) de 0 a 255
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

// Recua e gira para o lado oposto ao sensor que viu a borda
void inWhiteLine() {
  bool bordaNaEsquerda = ehBranco(sensorEsqValue);

  motores(-1, -1); // ré
  delay(tempoRe);

  if (bordaNaEsquerda) {
    motores(1, -1); // gira para a direita
  }
  else {
    motores(-1, 1); // gira para a esquerda
  }
  delay(tempoGiro);
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
// FUNÇÕES DE SUPORTE DO SISTEMA (NÃO ALTERAR)
// =====================================================================================

// Muda a cor do LED RGB 
void colorLED(int red, int green, int blue) {
  analogWrite(LedR, red);
  analogWrite(LedG, green);
  analogWrite(LedB, blue);
}

// Trata a leitura do controle remoto via Infravermelho
void receptorIR()
{
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return;
    }

    // Descomente a linha 'if' e o '}' abaixo quando for para competir

    // if (IrReceiver.decodedIRData.command == 0x1C && IrReceiver.decodedIRData.protocol == 7) {
      roboLigado = !roboLigado;

      if (roboLigado) {
        tempoInicio = millis();
      }
      else {
        // Parada dos motores
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);

        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);  
      }
    // }

    IrReceiver.resume();
  }
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