#include <TimerOne.h>

const int buttonStartPin = 13;
const int buttonBluePin = 12;
const int buttonGreenPin = 11;
const int buttonRedPin = 10;

const int ledRedRGBPin = 5;
const int ledGreenRGBPin = 3;
const int ledBlueRGBPin = 4;

const int ledBluePin = 9;
const int ledRedPin = 8;
const int ledGreenPin = 7;
const int ledStarHeartPin = 6;

volatile bool debounceRed = false;
volatile bool debounceGreen = false;
volatile bool debounceBlue = false;
volatile bool debounceStart = false;

volatile bool stateRed = false;
volatile bool stateGreen = false;
volatile bool stateBlue = false;
volatile bool stateStarHeart = false;

void setup() {
  Serial.begin(9600);  // Inicializa la consola serie

  pinMode(buttonStartPin, INPUT_PULLUP);
  pinMode(buttonBluePin, INPUT_PULLUP);
  pinMode(buttonGreenPin, INPUT_PULLUP);
  pinMode(buttonRedPin, INPUT_PULLUP);

  pinMode(ledRedRGBPin, OUTPUT);
  pinMode(ledGreenRGBPin, OUTPUT);
  pinMode(ledBlueRGBPin, OUTPUT);

  pinMode(ledBluePin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledStarHeartPin, OUTPUT);

  // Inicializar todos los LEDs en apagado
  digitalWrite(ledRedRGBPin, LOW);
  digitalWrite(ledGreenRGBPin, LOW);
  digitalWrite(ledBlueRGBPin, LOW);

  digitalWrite(ledBluePin, LOW);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledStarHeartPin, LOW);

  // Configurar el temporizador para llamar a la función handleDebounce cada 10 ms
  Timer1.initialize(10000); // 10 ms
  Timer1.attachInterrupt(handleDebounce);
}

void loop() {
  checkButtonRed();
  checkButtonGreen();
  checkButtonBlue();
  checkButtonStart();
}

void checkButtonRed() {
  if (!debounceRed && digitalRead(buttonRedPin) == LOW) {
    debounceRed = true;
    stateRed = !stateRed;
    digitalWrite(ledRedPin, stateRed);
    digitalWrite(ledRedRGBPin, stateRed);
    Serial.print("Red LED state: ");
    Serial.println(stateRed);
  }
}

void checkButtonGreen() {
  if (!debounceGreen && digitalRead(buttonGreenPin) == LOW) {
    debounceGreen = true;
    stateGreen = !stateGreen;
    digitalWrite(ledGreenPin, stateGreen);
    digitalWrite(ledGreenRGBPin, stateGreen);
    Serial.print("Green LED state: ");
    Serial.println(stateGreen);
  }
}

void checkButtonBlue() {
  if (!debounceBlue && digitalRead(buttonBluePin) == LOW) {
    debounceBlue = true;
    stateBlue = !stateBlue;
    digitalWrite(ledBluePin, stateBlue);
    digitalWrite(ledBlueRGBPin, stateBlue);
    Serial.print("Blue LED state: ");
    Serial.println(stateBlue);
  }
}

void checkButtonStart() {
  if (!debounceStart && digitalRead(buttonStartPin) == LOW) {
    debounceStart = true;
    stateStarHeart = !stateStarHeart;
    digitalWrite(ledStarHeartPin, stateStarHeart);
    Serial.print("Star/Heart LED state: ");
    Serial.println(stateStarHeart);
  }
}

void handleDebounce() {
  static unsigned long lastDebounceRed = 0;
  static unsigned long lastDebounceGreen = 0;
  static unsigned long lastDebounceBlue = 0;
  static unsigned long lastDebounceStart = 0;

  if (debounceRed) {
    if (millis() - lastDebounceRed > 50) { // 50 ms debounce time
      if (digitalRead(buttonRedPin) == HIGH) {
        debounceRed = false;
        Serial.println("Red button debounce reset");
      }
      lastDebounceRed = millis();
    }
  }

  if (debounceGreen) {
    if (millis() - lastDebounceGreen > 50) { // 50 ms debounce time
      if (digitalRead(buttonGreenPin) == HIGH) {
        debounceGreen = false;
        Serial.println("Green button debounce reset");
      }
      lastDebounceGreen = millis();
    }
  }

  if (debounceBlue) {
    if (millis() - lastDebounceBlue > 50) { // 50 ms debounce time
      if (digitalRead(buttonBluePin) == HIGH) {
        debounceBlue = false;
        Serial.println("Blue button debounce reset");
      }
      lastDebounceBlue = millis();
    }
  }

  if (debounceStart) {
    if (millis() - lastDebounceStart > 50) { // 50 ms debounce time
      if (digitalRead(buttonStartPin) == HIGH) {
        debounceStart = false;
        Serial.println("Start button debounce reset");
      }
      lastDebounceStart = millis();
    }
  }
}
