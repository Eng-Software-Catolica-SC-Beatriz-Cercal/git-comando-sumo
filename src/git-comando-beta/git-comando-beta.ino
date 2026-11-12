// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// git-comando-beta.ino
// Variante "sniper" do git-comando-alpha: fica no lugar varrendo em zigue-zague,
// mira longe e só avança quando vê algo na frente.
// Autor: Equipe Git Comando
// =-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Motor A (Esquerdo)
#define ENA 5
#define IN1 6
#define IN2 7

// Motor B (Direito)
#define ENB 3
#define IN3 2
#define IN4 4

// Sensores
#define SHARP A0  // Proximidade (GP2Y0A21, 10-80 cm)
#define SE A1     // Linha esquerdo
#define SD A2     // Linha direito

// Ajustes - linha (iguais ao alpha)
int limiteBranco   = 200;
int velocidade     = 140;
int velocidadeGiro = 180;
int tempoFreio     = 150;
int tempoRe        = 450;
int tempoReLado    = 400;
int tempoGiro180   = 250;
int tempoGiroLado  = 220;

// Ajustes - sniper
int limiteOponente   = 250;  // mais baixo que o alpha (500) = enxerga mais longe -> CALIBRAR pelo Serial
int velocidadeAtaque = 255;
int velocidadeBusca  = 130;  // mais devagar que o alpha; se o robô não girar no tapete, suba
int tempoPerna       = 400;  // ms girando para cada lado no zigue-zague (define o tamanho do arco)

int valorEsq;
int valorDir;
int sharpValue;

// Depois de ver o oponente, ataca até achar a borda: colado (< 10 cm) a leitura do Sharp cai
bool atacando = false;

bool buscaDireita = true;
unsigned long inicioPerna = 0;
unsigned long duracaoPerna;

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

  // Primeira perna é meia, para o zigue-zague ficar centrado na direção inicial
  duracaoPerna = tempoPerna / 2;
  inicioPerna = millis();
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
    atacando = false;
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

    // Recomeça o zigue-zague centrado na nova direção
    duracaoPerna = tempoPerna / 2;
    inicioPerna = millis();
  }
  // ---------- 2) ALVO NA MIRA (ou já atacando) -> ATAQUE ----------
  else if (atacando || sharpValue > limiteOponente) {
    atacando = true;
    frente(velocidadeAtaque);
  }
  // ---------- 3) NADA -> ZIGUE-ZAGUE NO LUGAR ----------
  else {
    if (millis() - inicioPerna >= duracaoPerna) {
      buscaDireita = !buscaDireita;
      duracaoPerna = tempoPerna;
      inicioPerna = millis();
    }

    if (buscaDireita) {
      girarDireita(velocidadeBusca);
    } else {
      girarEsquerda(velocidadeBusca);
    }
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
