#include <TimerOne.h> // Librería para manejar Timer1

// Definición de pines de los botones
const int buttonStartPin = 13;
const int buttonBluePin = 12;
const int buttonGreenPin = 11;
const int buttonRedPin = 10;

// Definición de pines de los LEDs RGB
const int ledRedRGBPin = 5;
const int ledGreenRGBPin = 3;
const int ledBlueRGBPin = 4;

// Definición de pines de los LEDs unicolor
const int ledBluePin = 9;
const int ledRedPin = 8;
const int ledGreenPin = 7;
const int ledStarHeartPin = 6; // LED del corazón

// Variables de debounce para los botones
volatile bool debounceRed = false;
volatile bool debounceGreen = false;
volatile bool debounceBlue = false;
volatile bool debounceStart = false;

// Estados de los botones
bool buttonStates[3] = {false, false, false}; // [red, green, blue]
bool playing = false; // Indica si el juego está en progreso
bool gameStarted = false; // Indica si el juego ha comenzado
bool gameLost = false; // Indica si el juego se ha perdido

int currentColor; // Almacena el color actual mostrado por los LEDs RGB
int roundsWon = 0; // Contador de rondas ganadas
const int totalRounds = 2; // Total de rondas a jugar

// Temporizador para cada ronda
unsigned long roundStartTime; // Tiempo de inicio de la ronda
const unsigned long roundDuration = 5000; // Duración de la ronda en milisegundos (5 segundos)

void setup() {
  Serial.begin(9600);  // Inicializa la consola serie

  // Configuración de pines de entrada para los botones
  pinMode(buttonStartPin, INPUT_PULLUP);
  pinMode(buttonBluePin, INPUT_PULLUP);
  pinMode(buttonGreenPin, INPUT_PULLUP);
  pinMode(buttonRedPin, INPUT_PULLUP);

  // Configuración de pines de salida para los LEDs RGB
  pinMode(ledRedRGBPin, OUTPUT);
  pinMode(ledGreenRGBPin, OUTPUT);
  pinMode(ledBlueRGBPin, OUTPUT);

  // Configuración de pines de salida para los LEDs unicolor
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

  // Inicializar el generador de números aleatorios
  randomSeed(analogRead(0));
}

// Función para establecer el color de los LEDs RGB
void setRGBColor(int red, int green, int blue) {
  digitalWrite(ledRedRGBPin, red);
  digitalWrite(ledGreenRGBPin, green);
  digitalWrite(ledBlueRGBPin, blue);
}

// Función para establecer el estado de los LEDs unicolor
void setUnicolorLEDs(int red, int green, int blue) {
  digitalWrite(ledRedPin, red);
  digitalWrite(ledGreenPin, green);
  digitalWrite(ledBluePin, blue);
}

// Función para establecer el estado del LED del corazón
void setHeartLED(int state) {
  digitalWrite(ledStarHeartPin, state);
}

// Función para hacer parpadear los LEDs RGB y unicolor junto con el LED del corazón
void blinkRGB(int red, int green, int blue, int times, int duration) {
  for (int i = 0; i < times; i++) {
    setRGBColor(red, green, blue);
    setUnicolorLEDs(red, green, blue);
    setHeartLED(HIGH);
    delay(duration);
    setRGBColor(LOW, LOW, LOW);
    setUnicolorLEDs(LOW, LOW, LOW);
    setHeartLED(LOW);
    delay(duration);
  }
}

// Función para seleccionar y mostrar un color aleatorio en los LEDs RGB
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

// Función para celebrar una victoria
void celebrateWin() {
  for (int i = 0; i < 3; i++) {
    setRGBColor(HIGH, LOW, LOW); // Rojo
    setUnicolorLEDs(HIGH, LOW, LOW); // Rojo
    setHeartLED(HIGH);
    delay(250);
    setRGBColor(LOW, HIGH, LOW); // Verde
    setUnicolorLEDs(LOW, HIGH, LOW); // Verde
    setHeartLED(LOW);
    delay(250);
    setRGBColor(LOW, LOW, HIGH); // Azul
    setUnicolorLEDs(LOW, LOW, HIGH); // Azul
    setHeartLED(HIGH);
    delay(250);
  }
  setRGBColor(LOW, LOW, LOW); // Apagar
  setUnicolorLEDs(LOW, LOW, LOW); // Apagar
  setHeartLED(LOW); // Apagar corazón
}

// Función para celebrar una derrota
void celebrateLoss() {
  for (int i = 0; i < 3; i++) {
    setRGBColor(HIGH, HIGH, HIGH); // Blanco (rojo, verde y azul encendidos a la vez)
    setUnicolorLEDs(HIGH, HIGH, HIGH); // Blanco (rojo, verde y azul encendidos a la vez)
    setHeartLED(HIGH);
    delay(250);
    setRGBColor(LOW, LOW, LOW); // Apagar
    setUnicolorLEDs(LOW, LOW, LOW); // Apagar
    setHeartLED(LOW);
    delay(250);
  }
  setHeartLED(LOW); // Asegurarse de que el corazón esté apagado
}

// Función para verificar si la selección es correcta
bool isCorrectSelection() {
  if (currentColor == 1 && digitalRead(ledRedPin) == HIGH) return true;
  if (currentColor == 2 && digitalRead(ledGreenPin) == HIGH) return true;
  if (currentColor == 3 && digitalRead(ledBluePin) == HIGH) return true;
  if (currentColor == 4 && digitalRead(ledRedPin) == HIGH && digitalRead(ledGreenPin) == HIGH) return true; // Amarillo
  if (currentColor == 5 && digitalRead(ledRedPin) == HIGH && digitalRead(ledBluePin) == HIGH) return true; // Morado
  if (currentColor == 6 && digitalRead(ledRedPin) == HIGH && digitalRead(ledGreenPin) == HIGH && digitalRead(ledBluePin) == HIGH) return true; // Blanco
  return false;
}

// Función para finalizar la ronda
void endRound(bool success) {
  setRGBColor(LOW, LOW, LOW); // Apagar LED RGB inmediatamente
  setUnicolorLEDs(LOW, LOW, LOW); // Apagar LEDs unicolor inmediatamente
  if (success) {
    blinkRGB(LOW, HIGH, LOW, 3, 250); // Parpadea en verde 3 veces
    roundsWon++;
    if (roundsWon >= totalRounds) {
      celebrateWin();
      playing = false;
      setHeartLED(LOW); // Apagar el corazón después de la celebración
    } else {
      randomColor(); // Continuar con la siguiente ronda
      roundStartTime = millis(); // Reiniciar el temporizador para la nueva ronda
    }
    Serial.println("Correct selection");
  } else {
    blinkRGB(HIGH, LOW, LOW, 3, 250); // Parpadea en rojo 3 veces
    setHeartLED(LOW); // Apagar el corazón si la selección es incorrecta
    celebrateLoss();
    playing = false;
    gameLost = true;
    Serial.println("Incorrect selection");
  }
}

// Función para iniciar el juego
void startGame() {
  playing = true;
  gameLost = false; // Resetear el indicador de pérdida al iniciar un nuevo juego
  roundsWon = 0; // Resetear el contador de rondas ganadas al iniciar un nuevo juego
  // Apagar todos los LEDs unicolor antes de comenzar el juego
  setUnicolorLEDs(LOW, LOW, LOW);
  blinkRGB(HIGH, HIGH, HIGH, 3, 500); // Parpadea en blanco 3 veces
  setRGBColor(LOW, LOW, LOW); // Asegurarse de que los LEDs RGB estén apagados después del parpadeo
  randomColor(); // Selecciona y muestra un color aleatorio
  setHeartLED(HIGH); // Enciende el corazón al iniciar el juego
  roundStartTime = millis(); // Inicializa el temporizador para la primera ronda
  Serial.println("Game started");
}

// Función para verificar el estado del botón rojo
void checkButtonRed() {
  if (!debounceRed && digitalRead(buttonRedPin) == LOW) {
    debounceRed = true;
    buttonStates[0] = !buttonStates[0];
    digitalWrite(ledRedPin, buttonStates[0]);
    Serial.print("Red LED state: ");
    Serial.println(buttonStates[0]);
  }
}

// Función para verificar el estado del botón verde
void checkButtonGreen() {
  if (!debounceGreen && digitalRead(buttonGreenPin) == LOW) {
    debounceGreen = true;
    buttonStates[1] = !buttonStates[1];
    digitalWrite(ledGreenPin, buttonStates[1]);
    Serial.print("Green LED state: ");
    Serial.println(buttonStates[1]);
  }
}

// Función para verificar el estado del botón azul
void checkButtonBlue() {
  if (!debounceBlue && digitalRead(buttonBluePin) == LOW) {
    debounceBlue = true;
    buttonStates[2] = !buttonStates[2];
    digitalWrite(ledBluePin, buttonStates[2]);
    Serial.print("Blue LED state: ");
    Serial.println(buttonStates[2]);
  }
}

// Función para verificar el estado del botón de inicio
void checkButtonStart() {
  if (!debounceStart && digitalRead(buttonStartPin) == LOW) {
    debounceStart = true;
    if (!playing) {
      if (gameLost) { // Si el juego se ha perdido, reiniciar el juego
        setHeartLED(LOW); // Asegurarse de que el corazón esté apagado antes de reiniciar
      }
      startGame();
    } else {
      bool isCorrect = isCorrectSelection();
      endRound(isCorrect);
    }
  }
}

// Función para manejar el debounce de los botones
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

void loop() {
  if (!playing) {
    if (digitalRead(buttonStartPin) == LOW) {
      checkButtonStart();
    }
  } else {
    checkButtonRed();
    checkButtonGreen();
    checkButtonBlue();
    if (digitalRead(buttonStartPin) == LOW) {
      checkButtonStart();
    }

    // Verificar si ha pasado el tiempo de la ronda
    if (millis() - roundStartTime > roundDuration) {
      endRound(false); // Si ha pasado el tiempo y no se ha seleccionado una respuesta correcta, se pierde la ronda
    }
  }
}
