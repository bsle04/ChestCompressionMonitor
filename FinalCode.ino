#include <LiquidCrystal.h>

// Pin assignments
const int buttonPin = 8;        // Button for compressions
const int depthPotPin = A1;     // Potentiometer for depth simulation
const int greenLED = 9;         // Green LED for correct rate
const int redLED = 10;          // Red LED for incorrect rate
const int buzzerPin = 7;        // Buzzer for feedback
const int depthGreenLED = 11;   // Depth Green LED
const int depthRedLED = 12;     // Depth Red LED
const int rgbRed = 6;           // RGB Red pin
const int rgbGreen = 5;         // RGB Green pin
const int rgbBlue = 4;          // RGB Blue pin
const int difficultyButton = 3; // Button for cycling difficulty levels

// Variables for compression rate
unsigned long lastPressTime = 0;
unsigned long currentPressTime = 0;
int pressCount = 0;
float calculatedCPM = 0;        // Compressions per minute
int targetCPM = 100;            // Default target CPM, can be adjusted via potentiometer

// Depth range parameters (simulate 4-5 cm depth range)
int minDepth = 400;       // Minimum depth (simulate 4 cm)
int maxDepth = 500;       // Maximum depth (simulate 5 cm)

// Variables for difficulty levels
int difficulty = 0;             // 0 = Beginner, 1 = Intermediate, 2 = Advanced
int lastDifficultyButtonState = LOW;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // Initialize LCD (RS, Enable, D4, D5, D6, D7)

void setup() {
  // put your setup code here, to run once:
  // Initialize pins
  pinMode(buttonPin, INPUT);      // Button input pin
  pinMode(depthPotPin, INPUT);
  pinMode(greenLED, OUTPUT);      // Green LED pin for correct rate
  pinMode(redLED, OUTPUT);        // Red LED pin for incorrect rate
  pinMode(buzzerPin, OUTPUT);     // Buzzer pin for feedback
  pinMode(depthGreenLED, OUTPUT); // Depth Green LED pin
  pinMode(depthRedLED, OUTPUT);   // Depth Red LED pin
  pinMode(rgbRed, OUTPUT);
  pinMode(rgbGreen, OUTPUT);
  pinMode(rgbBlue, OUTPUT);
  pinMode(difficultyButton, INPUT_PULLUP); // Button with internal pull-up
  
  // Initialize LCD
  lcd.begin(16, 2);               // LCD setup (16 columns, 2 rows)
  lcd.print("CPR Monitor");       // Display initial message
  delay(1000);

  updateDifficultyColor(difficulty); // Set initial difficulty level color
}

void loop() {
  // put your main code here, to run repeatedly:
  // Read difficulty button state
  int difficultyButtonState = digitalRead(difficultyButton);
  if (difficultyButtonState == HIGH && lastDifficultyButtonState == LOW) {
    difficulty = (difficulty + 1) % 3; // Cycle through 0, 1, 2
    updateDifficultyColor(difficulty); // Update RGB LED for difficulty
    delay(200); // Debounce
  }
  lastDifficultyButtonState = difficultyButtonState;

  // Update target CPM based on difficulty level
  switch (difficulty) {
    case 0: // Beginner
      targetCPM = 80;
      minDepth = 400; maxDepth = 500; // 4-5 cm
      break;
    case 1: // Intermediate
      targetCPM = 100;
      minDepth = 450; maxDepth = 500; // 4.5-5 cm
      break;
    case 2: // Advanced
      targetCPM = 120;
      minDepth = 475; maxDepth = 525; // 4.75-5.25 cm
      break;
  }

  // Read the button state to detect compression presses
  int buttonState = digitalRead(buttonPin);

  // Get the current time
  currentPressTime = millis();
  
  if (buttonState == HIGH) {
    if (currentPressTime - lastPressTime >= 600) { // Prevent multiple presses too quickly
      pressCount++;
      lastPressTime = currentPressTime;
    }
  }

  // Calculate CPM (compressions per minute); + 1 is added to prevent mathematical erros from dividing by 0
  calculatedCPM = pressCount * 60000.0 / (currentPressTime - lastPressTime + 1);

  // Read depth value from second potentiometer
  int depthValue = analogRead(depthPotPin);
  int depth = map(depthValue, 0, 1023, 0, 1000); // Scale to match depth range

  // Display the current CPM and target CPM on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Current CPM: ");
  lcd.print(calculatedCPM);
  
  lcd.setCursor(0, 1);
  lcd.print("Target: ");
  lcd.print(targetCPM);

  // Depth feedback
  if (depth >= minDepth && depth <= maxDepth) {
    digitalWrite(depthGreenLED, HIGH);  // Depth is correct
    digitalWrite(depthRedLED, LOW);
    tone(buzzerPin, 1000, 200);  // Short beep for correct depth
  } else {
    digitalWrite(depthGreenLED, LOW);   // Depth is incorrect
    digitalWrite(depthRedLED, HIGH);
    tone(buzzerPin, 300, 500);   // Continuous tone for incorrect depth
  }

  // Rate feedback (CPM check)
  if (calculatedCPM < targetCPM - 5) {
    // Too slow: red LED, buzzer continuous low tone
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    tone(buzzerPin, 500, 500); // Continuous low tone
  } else if (calculatedCPM > targetCPM + 5) {
    // Too fast: red LED, buzzer continuous high tone
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    tone(buzzerPin, 1500, 500); // Continuous high tone
  } else {
    // Correct rate: green LED, short beep
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    tone(buzzerPin, 1000, 200); // Short beep
  }

  delay(200); // Short delay to prevent rapid feedback changes
}

// Function to update RGB LED color for difficulty level
void updateDifficultyColor(int level) {
  // Turn off all colors
  digitalWrite(rgbRed, LOW);
  digitalWrite(rgbGreen, LOW);
  digitalWrite(rgbBlue, LOW);

  switch (level) {
    case 0: // Beginner
      digitalWrite(rgbRed, HIGH); // Red color
      break;
    case 1: // Intermediate
      digitalWrite(rgbGreen, HIGH); // Green color
      break;
    case 2: // Advanced
      digitalWrite(rgbBlue, HIGH); // Blue color
      break;
  }
}
