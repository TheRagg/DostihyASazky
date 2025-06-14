#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// put function declarations here:

LiquidCrystal_I2C lcd(0x27, 16, 2);

int red1 = 22;
int red2 = 23;
int red3 = 24;
int red4 = 25;
int red5 = 26;
int blue1 = 27;
int blue2 = 28;
int blue3 = 29;
int blue4 = 30;
int blue5 = 31;
int yellow1 = 32;
int yellow2 = 33;
int yellow3 = 34;
int yellow4 = 35;
int yellow5 = 36;
int green1 = 37;
int green2 = 38;
int green3 = 39;
int green4 = 40;
int green5 = 41;
int relayPin = 50;

int redArray[] = {red1, red2, red3, red4, red5};
int blueArray[] = {blue1, blue2, blue3, blue4, blue5};
int yellowArray[] = {yellow1, yellow2, yellow3, yellow4, yellow5};
int greenArray[] = {green1, green2, green3, green4, green5};

const int ARRAY_SIZE = 5;
const int NUM_ARRAYS = 4;

// Timing and state variables for each array
unsigned long previousMillis[NUM_ARRAYS] = {0, 0, 0, 0};
int currentLED[NUM_ARRAYS] = {0, 0, 0, 0};
unsigned long arrayDelays[NUM_ARRAYS] = {0, 0, 0, 0};
bool arrayFinished[NUM_ARRAYS] = {false, false, false, false};
bool raceStarted = false;
bool raceFinished = false;
int winner = -1;

const int BUTTON_PIN = 51;
bool lastButtonState = HIGH;
bool buttonPressed = false;

// Array names for serial output
String arrayNames[] = {"RED", "BLUE", "YELLOW", "GREEN"};

// Function to get array pointer by index
int* getArrayByIndex(int index) {
  switch (index) {
    case 0: return redArray;
    case 1: return blueArray;
    case 2: return yellowArray;
    case 3: return greenArray;
    default: return redArray;
  }
}

// Function to turn off all LEDs
void turnOffAllLEDs() {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    digitalWrite(redArray[i], LOW);
    digitalWrite(blueArray[i], LOW);
    digitalWrite(yellowArray[i], LOW);
    digitalWrite(greenArray[i], LOW);
  }
}

// Function to start a new race
void startNewRace() {
  Serial.println("\n=== STARTING NEW LED RACE ===");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("START!");
  digitalWrite(relayPin, HIGH);
  
  // Reset all states
  for (int i = 0; i < NUM_ARRAYS; i++) {
    currentLED[i] = 0;
    arrayFinished[i] = false;
    previousMillis[i] = millis();
    
    // Set random delay for each array
    arrayDelays[i] = random(100, 1800);
    
    Serial.print(arrayNames[i]);
    Serial.print(" array delay: ");
    Serial.print(arrayDelays[i]);
    Serial.println("ms");
  }
  
  // Turn off all LEDs
  turnOffAllLEDs();
  
  raceStarted = true;
  raceFinished = false;
  winner = -1;
  
  Serial.println("Race started! Watch the LEDs...\n");
}

// Function to check if any array won
void checkForWinner() {
  if (raceFinished) return;
  
  for (int i = 0; i < NUM_ARRAYS; i++) {
    if (arrayFinished[i] && winner == -1) {
      winner = i;
      raceFinished = true;
      
      digitalWrite(relayPin, LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("WINNER:");
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
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < ARRAY_SIZE; i++) {
    pinMode(redArray[i], OUTPUT);
    pinMode(blueArray[i], OUTPUT);
    pinMode(yellowArray[i], OUTPUT);
    pinMode(greenArray[i], OUTPUT);
  }

  pinMode(relayPin, OUTPUT);
  randomSeed(analogRead(0));

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Dostihy a sazky");
  lcd.setCursor(0,1);
  lcd.print("Trochu gamba xd");

  // Wait a moment for pin to stabilize, then read initial button state
  delay(100);
  lastButtonState = digitalRead(BUTTON_PIN);
  
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("READY");
  Serial.println("Dostihy a sazky");
  Serial.println("Press the button on pin 51 to start a race!");
  Serial.println("(You can also press any key in Serial Monitor)");
  
  
}

void loop() {
  
  // Check button state for debounced button press
  bool currentButtonState = digitalRead(BUTTON_PIN);
  
  // Detect button press (HIGH to LOW transition with debouncing)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50); // Simple debounce delay
    if (digitalRead(BUTTON_PIN) == LOW) { // Confirm button is still pressed
      buttonPressed = true;
    }
  }
  lastButtonState = currentButtonState;
  
  // Check for serial input to start new race (alternative trigger)
  if (Serial.available() > 0) {
    Serial.read(); // Clear the serial buffer
    buttonPressed = true;
  }
  
  // Start new race if button was pressed
  if (buttonPressed) {
    buttonPressed = false; // Reset flag
    startNewRace();
  }
  
  // Only run race logic if race is active
  if (!raceStarted || raceFinished) {
    return;
  }
  
  unsigned long currentMillis = millis();
  
  // Process each array independently
  for (int arrayIndex = 0; arrayIndex < NUM_ARRAYS; arrayIndex++) {
    // Skip if this array is already finished
    if (arrayFinished[arrayIndex]) {
      continue;
    }
    
    // Check if it's time for this array's next LED
    if (currentMillis - previousMillis[arrayIndex] >= arrayDelays[arrayIndex]) {
      previousMillis[arrayIndex] = currentMillis;
      
      // Get the current array
      int* activeArray = getArrayByIndex(arrayIndex);
      
      // Turn on the current LED for this array
      digitalWrite(activeArray[currentLED[arrayIndex]], HIGH);
      
      Serial.print(arrayNames[arrayIndex]);
      Serial.print(" LED ");
      Serial.print(currentLED[arrayIndex] + 1);
      Serial.print("/");
      Serial.print(ARRAY_SIZE);
      Serial.println(" ON");
      
      // Move to next LED in this array
      currentLED[arrayIndex]++;
      
      // Check if this array is complete
      if (currentLED[arrayIndex] >= ARRAY_SIZE) {
        arrayFinished[arrayIndex] = true;
        Serial.print(">>> ");
        Serial.print(arrayNames[arrayIndex]);
        Serial.println(" ARRAY COMPLETE! <<<");
      }
    }
  }
  
  // Check for winner after processing all arrays
  checkForWinner();

}