#include <TimerOne.h>

const int buttonStartPin = 13;
const int buttonRedPin = 12;
const int buttonBluePin = 11;
const int buttonGreenPin = 10;
const int ledRedPin = 9;
const int ledBluePin = 8;
const int ledGreenPin = 7;
const int heartLedPin = 6;
const int ledRedRGBPin = 5;
const int ledGreenRGBPin = 3;
const int ledBlueRGBPin = 4;
int counter = 0;
int counter2 = 0;
int endGame_Timer = 0;
int endGame_Flag = 0;

const int maxRounds = 10; // Máximo de rondas
int roundTime = 10; // segundos por ronda

int currentRound = 1;
bool playing = false;
int currentColor; // Variable para almacenar el color actual mostrado
bool buttonStates[3] = {false, false, false}; // Estados de los botones

void timerISR() {
  if(endGame_Timer == 1)
  {
    counter2++;
    if(counter2 == 250)
    {
    endGame_Flag = 1;
    endGame_Timer = 0;
    }
  }
  counter++;  // Increment the counter every 1ms
}

void setup() {
  Serial.begin(9600);
  // Initialize Timer1
  Timer1.initialize(1000);  // Set a period of 1000 microseconds (1 millisecond)
  Timer1.attachInterrupt(timerISR);  // Attach the ISR (interrupt service routine)
  pinMode(buttonStartPin, INPUT);
  pinMode(buttonRedPin, INPUT);
  pinMode(buttonBluePin, INPUT);
  pinMode(buttonGreenPin, INPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedRGBPin, OUTPUT);
  pinMode(ledGreenRGBPin, OUTPUT);
  pinMode(ledBlueRGBPin, OUTPUT);
  pinMode(heartLedPin, OUTPUT);
}

void setRGBColor(int red, int green, int blue) {
  digitalWrite(ledRedRGBPin, red);
  digitalWrite(ledGreenRGBPin, green);
  digitalWrite(ledBlueRGBPin, blue);
}

void blinkRGB(int red, int green, int blue, int times, int duration) {
  for (int i = 0; i < times; i++) {
    setRGBColor(red, green, blue);
    endGame_Flag = 0;
    endGame_Timer = 1;
    while(endGame_Flag);
    setRGBColor(LOW, LOW, LOW);
    endGame_Flag = 0;
    endGame_Timer = 1;
    while(endGame_Flag);
  }
}

void randomColor() {
  currentColor = random(1, 7); // Colores del 1 al 6
  switch (currentColor) {
    case 1: setRGBColor(HIGH, LOW, LOW); break; // Rojo
    case 2: setRGBColor(LOW, HIGH, LOW); break; // Verde
    case 3: setRGBColor(LOW, LOW, HIGH); break; // Azul
    case 4: setRGBColor(HIGH, HIGH, LOW); break; // Amarillo
    case 5: setRGBColor(HIGH, LOW, HIGH); break; // Morado
    case 6: setRGBColor(HIGH, HIGH, HIGH); break; // Blanco
  }
}

void startGame() {
  playing = true;
  currentRound = 1;
  digitalWrite(heartLedPin, HIGH);  // Asegura que el corazón esté encendido
  blinkRGB(HIGH, HIGH, HIGH, 3, 500); // Parpadea en blanco 3 veces
  randomColor(); // Selecciona y muestra un color aleatorio
}

void startRound() {
  buttonStates[0] = buttonStates[1] = buttonStates[2] = false; // Resetear estados de los botones
  digitalWrite(ledRedPin, LOW);  // Apagar LEDs unicolor al iniciar una nueva ronda
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledBluePin, LOW);
  blinkRGB(HIGH, HIGH, HIGH, 3, 500); // Parpadea en blanco 3 veces
  randomColor(); // Selecciona y muestra un color aleatorio
}

void endRound(bool success) {
  setRGBColor(LOW, LOW, LOW);  // Apagar LED RGB inmediatamente
  buttonStates[0] = buttonStates[1] = buttonStates[2] = false; // Resetear estados de los botones

  if (success) {
    blinkRGB(LOW, HIGH, LOW, 3, 250); // Parpadea en verde 3 veces
    if (currentRound < maxRounds) {
      currentRound++;
      startRound(); // Iniciar una nueva ronda
    } else {
      playing = false;
      digitalWrite(heartLedPin, LOW);
    }
  } else {
    endGame();
  }
}

void endGame() {
    // Parpadea en rojo 3 veces
    for (int i = 0; i < 3; i++) {
      setRGBColor(HIGH, LOW, LOW);
      delay(250);
      setRGBColor(LOW, LOW, LOW);
      delay(250);
    }
    playing = false;
    digitalWrite(heartLedPin, LOW);
}

bool isCorrectSelection() {
  // Verifica si la selección es correcta
  if (currentColor == 1 && digitalRead(ledRedPin) == HIGH) return true;
  if (currentColor == 2 && digitalRead(ledGreenPin) == HIGH) return true;
  if (currentColor == 3 && digitalRead(ledBluePin) == HIGH) return true;
  if (currentColor == 4 && digitalRead(ledRedPin) == HIGH && digitalRead(ledGreenPin) == HIGH) return true; // Amarillo
  if (currentColor == 5 && digitalRead(ledRedPin) == HIGH && digitalRead(ledBluePin) == HIGH) return true; // Morado
  if (currentColor == 6 && digitalRead(ledRedPin) == HIGH && digitalRead(ledGreenPin) == HIGH && digitalRead(ledBluePin) == HIGH) return true; // Blanco
  return false;
}

void loop() {
  if (!playing && digitalRead(buttonStartPin) == HIGH) {
    startGame();
  }

  if (playing) {

    // Leer el estado de los botones
    bool redButton = digitalRead(buttonRedPin) == HIGH;
    bool greenButton = digitalRead(buttonGreenPin) == HIGH;
    bool blueButton = digitalRead(buttonBluePin) == HIGH;

    // Controlar los LEDs unicolor como interruptores
    if (redButton && !buttonStates[0]) {
      buttonStates[0] = true;
      digitalWrite(ledRedPin, !digitalRead(ledRedPin));
    } else if (!redButton) {
      buttonStates[0] = false;
    }

    if (greenButton && !buttonStates[1]) {
      buttonStates[1] = true;
      digitalWrite(ledGreenPin, !digitalRead(ledGreenPin));
    } else if (!greenButton) {
      buttonStates[1] = false;
    }

    if (blueButton && !buttonStates[2]) {
      buttonStates[2] = true;
      digitalWrite(ledBluePin, !digitalRead(ledBluePin));
    } else if (!blueButton) {
      buttonStates[2] = false;
    }

    // Verificar si el jugador ha presionado el botón de inicio para confirmar su selección
    if (digitalRead(buttonStartPin) == HIGH) {
      bool isCorrect = isCorrectSelection();
      endRound(isCorrect);
    }
  }
}
