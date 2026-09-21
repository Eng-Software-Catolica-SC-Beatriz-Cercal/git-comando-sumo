// T1 - teste dos motores e dos sensores (Serial Monitor em 9600)
// 1) Com o robô levantado: gira cada roda pra frente e pra trás, uma vez só.
// 2) Depois, com o robô no chão: mostra os valores dos sensores sem parar.
// Pra repetir o teste dos motores, apertar o RESET do Arduino.

#define ENA 5
#define IN1 6
#define IN2 7
#define ENB 3
#define IN3 2
#define IN4 4

#define SHARP A0
#define SE A1
#define SD A2

int velocidade = 180;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.begin(9600);

  Serial.println("Levante o robô, os motores ligam em 3 s");
  delay(3000);

  Serial.println("Esquerdo pra frente");
  motores(velocidade, 0);
  delay(2000);

  Serial.println("Esquerdo pra trás");
  motores(-velocidade, 0);
  delay(2000);

  Serial.println("Direito pra frente");
  motores(0, velocidade);
  delay(2000);

  Serial.println("Direito pra trás");
  motores(0, -velocidade);
  delay(2000);

  motores(0, 0);
  Serial.println("Fim dos motores. Pode colocar o robô no chão.");
}

void loop() {
  Serial.print("Esquerdo: ");
  Serial.print(analogRead(SE));
  Serial.print("   Direito: ");
  Serial.print(analogRead(SD));
  Serial.print("   Distância: ");
  Serial.println(analogRead(SHARP));
  delay(300);
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
