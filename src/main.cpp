#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// piny posuvného registru
const int DATA_PIN = 2;   // DS (Data pin)
const int CLOCK_PIN = 3;  // SHCP (Clock pin)
const int LATCH_PIN = 4;  // STCP (Latch pin)

const int relayPin = 50;
const int BUTTON_PIN = 51;

// LED mapping pro bity posuvných registrů
// červené LED
const int red1 = 0;
const int red2 = 1;
const int red3 = 2;
const int red4 = 3;
const int red5 = 4;

// modré LED
const int blue1 = 5;
const int blue2 = 6;
const int blue3 = 7;
const int blue4 = 8;
const int blue5 = 9;

// žluté LED
const int yellow1 = 10;
const int yellow2 = 11;
const int yellow3 = 12;
const int yellow4 = 13;
const int yellow5 = 14;

// zelené LED
const int green1 = 15;
const int green2 = 16;
const int green3 = 17;
const int green4 = 18;
const int green5 = 19;

int redArray[] = {red1, red2, red3, red4, red5};
int blueArray[] = {blue1, blue2, blue3, blue4, blue5};
int yellowArray[] = {yellow1, yellow2, yellow3, yellow4, yellow5};
int greenArray[] = {green1, green2, green3, green4, green5};

const int ARRAY_SIZE = 5;
const int NUM_ARRAYS = 4;

// paměť stavů
byte ledStates[3] = {0, 0, 0};

// časování a proměný pro každý array
unsigned long previousMillis[NUM_ARRAYS] = {0, 0, 0, 0};
int currentLED[NUM_ARRAYS] = {0, 0, 0, 0};
unsigned long arrayDelays[NUM_ARRAYS] = {0, 0, 0, 0};
bool arrayFinished[NUM_ARRAYS] = {false, false, false, false};
bool raceStarted = false;
bool raceFinished = false;
int winner = -1;

bool lastButtonState = HIGH;
bool buttonPressed = false;

// nastavení rychlosti hry
const int MIN_DELAY = 200;
const int MAX_DELAY = 1200;
const int SPEED_CHANGE_PERCENT = 40;

String arrayNames[] = {"CERVENA", "MODRA", "ZLUTA", "ZELENA"};

// aktualizace posuvných registrů se stavem LED
void updateShiftRegisters() {
  digitalWrite(LATCH_PIN, LOW);
  
  // posunutí všech 3 bytů - MSB první
  // začít od konce
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, ledStates[2]);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, ledStates[1]);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, ledStates[0]);
  
  digitalWrite(LATCH_PIN, HIGH);
}

// funkce na zapnutí / vypnutí specifické ledky
void setLED(int ledBit, bool state) {
  int byteIndex = ledBit / 8;      // byte (0,1,2)
  int bitPosition = ledBit % 8;     // bit (0-7)
  
  if (state) {
    ledStates[byteIndex] |= (1 << bitPosition);
  } else {
    ledStates[byteIndex] &= ~(1 << bitPosition);
  }
  
  updateShiftRegisters();
}

// funkce na získání ukazatele arraye z indexu
int* getArrayByIndex(int index) {
  switch (index) {
    case 0: return redArray;
    case 1: return blueArray;
    case 2: return yellowArray;
    case 3: return greenArray;
    default: return redArray;
  }
}

// funkce na vypnutí všech ledek
void turnOffAllLEDs() {
  // smazání stavu
  ledStates[0] = 0;
  ledStates[1] = 0;
  ledStates[2] = 0;
  updateShiftRegisters();
}

// funkce na vygenerování náhodného seedu pro array
void randomizeArraySpeed(int arrayIndex) {
  int currentDelay = arrayDelays[arrayIndex];
  int variation = (currentDelay * SPEED_CHANGE_PERCENT) / 100;
  
  int minNewDelay = currentDelay - variation;
  int maxNewDelay = currentDelay + variation;
  
  minNewDelay = constrain(minNewDelay, MIN_DELAY, MAX_DELAY);
  maxNewDelay = constrain(maxNewDelay, MIN_DELAY, MAX_DELAY);
  
  arrayDelays[arrayIndex] = random(minNewDelay, maxNewDelay);
  
  Serial.print(arrayNames[arrayIndex]);
  Serial.print(" speed changed to: ");
  Serial.print(arrayDelays[arrayIndex]);
  Serial.println("ms");
}

// funkce na spuštění nového závodu
void startNewRace() {
  Serial.println("\n=== STARTING NEW LED RACE ===");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("START!");
  digitalWrite(relayPin, HIGH);
  
  // resetování všech stavů
  for (int i = 0; i < NUM_ARRAYS; i++) {
    currentLED[i] = 0;
    arrayFinished[i] = false;
    previousMillis[i] = millis();
    
    arrayDelays[i] = random(MIN_DELAY, MAX_DELAY);
    
    Serial.print(arrayNames[i]);
    Serial.print(" starting delay: ");
    Serial.print(arrayDelays[i]);
    Serial.println("ms");
  }
  
  // vypnutí všech LED
  turnOffAllLEDs();
  
  raceStarted = true;
  raceFinished = false;
  winner = -1;
  
  Serial.println("Race started! Watch the LEDs...\n");
}

// funkce na kontrolu výherce
void checkForWinner() {
  if (raceFinished) return;
  
  for (int i = 0; i < NUM_ARRAYS; i++) {
    if (arrayFinished[i] && winner == -1) {
      winner = i;
      raceFinished = true;
      
      digitalWrite(relayPin, LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("VYHERCE:");
      lcd.setCursor(0,1);
      lcd.print(arrayNames[i]);

      Serial.println("\n🏆 RACE FINISHED! 🏆");
      Serial.print("WINNER: ");
      Serial.print(arrayNames[i]);
      Serial.println(" ARRAY!");
      Serial.println("Press the button on pin 51 to start a new race!");
      Serial.println("(You can also press any key in Serial Monitor)\n");
      break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  
  turnOffAllLEDs();
  
  randomSeed(analogRead(0));

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Dostihy a sazky");
  lcd.setCursor(0,1);
  lcd.print("Trochu gamba xd");

  delay(100);
  lastButtonState = digitalRead(BUTTON_PIN);
  
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("PRIPRAVEN");
  Serial.println("Dostihy a sazky");
  Serial.println("Press the button on pin 51 to start a race!");
  Serial.println("(You can also press any key in Serial Monitor)");
}

void loop() {
  // debounce check
  bool currentButtonState = digitalRead(BUTTON_PIN);
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      buttonPressed = true;
    }
  }
  lastButtonState = currentButtonState;
  
  // kontrola serial vstupu
  if (Serial.available() > 0) {
    Serial.read();
    buttonPressed = true;
  }
  
  // začít závod pokuď je stistknuté tlačítko
  if (buttonPressed) {
    buttonPressed = false;
    startNewRace();
  }
  
  // spouštění logiky závodu jen tehdy když je závod aktivní
  if (!raceStarted || raceFinished) {
    return;
  }
  
  unsigned long currentMillis = millis();
  
  // zpracování každého arraye samostatně
  for (int arrayIndex = 0; arrayIndex < NUM_ARRAYS; arrayIndex++) {
    if (arrayFinished[arrayIndex]) {
      continue;
    }
    
    if (currentMillis - previousMillis[arrayIndex] >= arrayDelays[arrayIndex]) {
      previousMillis[arrayIndex] = currentMillis;
      
      int* activeArray = getArrayByIndex(arrayIndex);
      
      // zapnutí ledky pomocí posuvného registru
      setLED(activeArray[currentLED[arrayIndex]], HIGH);
      
      Serial.print(arrayNames[arrayIndex]);
      Serial.print(" LED ");
      Serial.print(currentLED[arrayIndex] + 1);
      Serial.print("/");
      Serial.print(ARRAY_SIZE);
      Serial.println(" ON");
      
      currentLED[arrayIndex]++;
      
      if (currentLED[arrayIndex] >= ARRAY_SIZE) {
        arrayFinished[arrayIndex] = true;
        Serial.print(">>> ");
        Serial.print(arrayNames[arrayIndex]);
        Serial.println(" ARRAY COMPLETE! <<<");
      } else {
        randomizeArraySpeed(arrayIndex);
      }
    }
  }
  
  checkForWinner();
}