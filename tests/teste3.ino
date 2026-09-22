// Base: tests/teste2.1.ino do repo (teste2 + controle IR no pino 8)
// Sem a linha abaixo o PWM do motor direito (pino 3) briga com o receptor IR,
// porque os dois usam o Timer2 do Arduino. Não apagar.
#define IR_USE_AVR_TIMER1
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
int sharpValue;
int sensorEsqValue;
int sensorDirValue;

// Configuracoes
const int threshold = 500;
const int line = 700;
const int tempoRe = 300;
const int tempoGiro = 400;
const float fatorVelocidade = 0.9;
const float forcaBusca = 0.7;
const float forcaAtaque = 1.0;

int estadoBorda = 0;
unsigned long tempoEstadoBorda = 0;
bool bordaNaEsquerda = false;
bool roboLigado = false;
unsigned long tempoInicio = 0;
unsigned long ultimoDebug = 0;

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
	pinMode(IR, INPUT);

	Serial.begin(9600);
	IrReceiver.begin(IR, DISABLE_LED_FEEDBACK);
	pararMotores();
	colorLED(0, 0, 0);
}

void loop() {
	// Deve ser chamado sem delay para nao perder o sinal do controle.
	receptorIR();

	if (!roboLigado) {
		verificaSensores();
		return;
	}

	if (millis() - tempoInicio < 5000) {
		pararMotores();
		return;
	}

	sharpValue = analogRead(SHARP);
	sensorEsqValue = analogRead(SE);
	sensorDirValue = analogRead(SD);

	if (estadoBorda != 0) {
		atualizaInWhiteLine();
		return;
	}

	if (sensorEsqValue < line || sensorDirValue < line) {
		inWhiteLine();
	}
	else if (sharpValue > threshold) {
		moveToOponent();
	}
	else {
		searchMethod();
	}
}

void inWhiteLine() {
	bordaNaEsquerda = sensorEsqValue < line;
	motores(-1.0, -1.0);
	estadoBorda = 1;
	tempoEstadoBorda = millis();
}

void atualizaInWhiteLine() {
	unsigned long agora = millis();

	if (estadoBorda == 1 && agora - tempoEstadoBorda >= tempoRe) {
		if (bordaNaEsquerda) {
			motores(1.0, -1.0);
		}
		else {
			motores(-1.0, 1.0);
		}
		estadoBorda = 2;
		tempoEstadoBorda = agora;
	}
	else if (estadoBorda == 2 && agora - tempoEstadoBorda >= tempoGiro) {
		estadoBorda = 0;
	}
}

void moveToOponent() {
	motores(forcaAtaque, forcaAtaque);
}

void searchMethod() {
	motores(forcaBusca, -forcaBusca);
}

void motores(float esquerdo, float direito) {
	int velEsq = (int)(constrain(abs(esquerdo), 0.0, 1.0) * 255.0 * fatorVelocidade);
	int velDir = (int)(constrain(abs(direito), 0.0, 1.0) * 255.0 * fatorVelocidade);

	if (esquerdo >= 0.0) {
		digitalWrite(IN1, HIGH);
		digitalWrite(IN2, LOW);
	}
	else {
		digitalWrite(IN1, LOW);
		digitalWrite(IN2, HIGH);
	}
	analogWrite(ENA, velEsq);

	if (direito >= 0.0) {
		digitalWrite(IN3, HIGH);
		digitalWrite(IN4, LOW);
	}
	else {
		digitalWrite(IN3, LOW);
		digitalWrite(IN4, HIGH);
	}
	analogWrite(ENB, velDir);
}

void pararMotores() {
	analogWrite(ENA, 0);
	analogWrite(ENB, 0);
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
	estadoBorda = 0;
}

void colorLED(int red, int green, int blue) {
	digitalWrite(LedR, red ? HIGH : LOW);
	digitalWrite(LedG, green ? HIGH : LOW);
	digitalWrite(LedB, blue ? HIGH : LOW);
}

void receptorIR() {
	if (!IrReceiver.decode()) {
		return;
	}

	bool repeticao = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
	if (!repeticao) {
		roboLigado = !roboLigado;

		if (roboLigado) {
			tempoInicio = millis();
			estadoBorda = 0;
		}
		else {
			pararMotores();
		}

		Serial.print("Robo ");
		Serial.println(roboLigado ? "ligado" : "desligado");
	}

	IrReceiver.resume();
}

void verificaSensores() {
	unsigned long agora = millis();
	if (agora - ultimoDebug < 500) {
		return;
	}
	ultimoDebug = agora;

	sharpValue = analogRead(SHARP);
	sensorEsqValue = analogRead(SE);
	sensorDirValue = analogRead(SD);

	Serial.print("SHARP: ");
	Serial.println(sharpValue);
	Serial.print("SE: ");
	Serial.println(sensorEsqValue);
	Serial.print("SD: ");
	Serial.println(sensorDirValue);
}
