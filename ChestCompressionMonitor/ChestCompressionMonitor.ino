#include <LiquidCrystal.h>
#include <math.h> //to use round function

//assigning pin variables
const int buttonPin = 8; //compression button
const int whiteLED = 9; //white led for correct rate
const int redLED = 6; //red led for incorrect rate
const int difficultyGreenLED = 10; //green LED for difficulty level
const int difficultyButton = 13; //button to cycle difficulty level
const int depthPot = A1; //potentiometer for depth simulator
const int buzzerPin = 7; //depth feedback buzzer

//miscellaneous variables (counters, button states, etc.)
int buttonState = HIGH; //current state of button
int lastButtonState = HIGH; //previous state of button
int buttonPressCount = 0; //counter for button presses
int difficulty = 0; //difficulty level 0, 1, 2
int lastDifficultyButtonState = LOW; //state for difficulty button
int targetCPM = 80; 
int minDepth = 400; //4cm simulated
int maxDepth = 500; //5cm simulated
float pressesPerMinute = 0; //variable for cpm
int depth = 0; //depth (will be read from potentiometer)

//time variables
unsigned long lastDebounceTime = 0; //the last time the button state changed
const unsigned long debounceDelay = 50; //debounce time in milliseconds
unsigned long startTime = 0; //start time for measuring presses
unsigned long updateInterval = 4000; //cpm is updated every 4 seconds / 4000 ms (to have a stable cpm w/o constant fluctuation) 

//intialize LCD (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); //setting button pin to input
  Serial.begin(9600); //start serial communication at 9600 baud
  startTime = millis(); //initialize start time
  
  lcd.begin(16, 2); //start lcd with 16 columns and 2 rows (LCD1602)
  lcd.print("CPR Monitor"); //display this string on the lcd
  delay(1000); //wait one second
  difficulty = 0; //start with 0 difficulty (easiest)
}

void loop() {
  //difficulty button (done)
  int difficultyButtonState = digitalRead(difficultyButton);
  if(difficultyButtonState == LOW && lastDifficultyButtonState == HIGH && (millis() - lastDebounceTime) > debounceDelay){ //if the button is pressed, uses debounce time to minimize duplicate presses
    if(difficulty != 2){ //if difficulty is not 2
      difficulty++; //increase by 1
    }else{ //otherwise, loop back over to 0
      difficulty = 0;
    }
    lastDebounceTime = millis();  //reset debounce timer
  }
  setLEDBrightness(difficulty); //update LED brightness
  lastDifficultyButtonState = difficultyButtonState;
  switch(difficulty){//switch case block which is cleaner code than a bunch of if else statements
    case 0: //if difficulty == 0
      targetCPM = 80;
      minDepth = 400; maxDepth = 500; // 4-5 cm
      break;
    case 1: //if difficulty == 1
      targetCPM = 100;
      minDepth = 450; maxDepth = 500; // 4.5-5 cm
      break;
    case 2: //if difficulty == 2
      targetCPM = 120;
      minDepth = 475; maxDepth = 525; // 4.75-5.25 cm
      break;
  }




  //cpm calculation (done)
  int reading = digitalRead(buttonPin);
  //check if the button state has changed
  if(reading != lastButtonState){
    lastDebounceTime = millis(); //reset the debounce timer, same debouncing logic as above
  }
  if((millis() - lastDebounceTime) > debounceDelay){
    if(reading != buttonState){
      buttonState = reading;
      if(buttonState == LOW){ //if button is pressed (checked if press is valid via debounce logic)
        buttonPressCount++; //increment press count
        Serial.print("Button press count: "); //print to serial monitor for debugging
        Serial.println(buttonPressCount);
        delay(100); //small delay to avoid glitches
      }
    }
  }
  if(millis() - startTime >= updateInterval){ //runs this block every four seconds or whatever updateInterval is set to
    pressesPerMinute = (buttonPressCount * 60000.0) / (millis() - startTime); //multiplies presses by 60,000 ms then divides it by the time elapsed since the last calculation
    Serial.print("Button presses per minute: "); //prints to serial for debugging
    Serial.println(pressesPerMinute);
    Serial.print("Depth: ");
    Serial.println(depth);

    //resets start time for the next updateInterval
    startTime = millis();
    buttonPressCount = 0;
  }



  //lcd displaying (done)
  lcd.setCursor(0, 0);
  lcd.print("                "); //clears the row
  lcd.setCursor(0, 0);
  lcd.print("Current CPM: "); //displays cpm calculated above
  lcd.print(int(round(pressesPerMinute)));
  
  lcd.setCursor(0, 1);
  lcd.print("                "); //clears the row
  lcd.setCursor(0, 1);
  lcd.print("Target: "); //displays targetCPM which depends on difficulty
  lcd.print(targetCPM);

  lastButtonState = reading; //updates the last button state, part of debounce logic




  //rate feedback (done)
  if(pressesPerMinute > (targetCPM - 15) && pressesPerMinute < (targetCPM + 15)){ //if the rate is within the target +- 15
    digitalWrite(whiteLED, HIGH); //turn on white LED and red off (good)
    digitalWrite(redLED, LOW);
  }else{
    digitalWrite(redLED, HIGH); //turn on red LED and white off (bad)
    digitalWrite(whiteLED, LOW);
  }




  //depth feedback (done)
  depth = map(analogRead(depthPot), 0, 1023, 0, 1000); //reads the potentiometer value and maps the value between 0-1000
  //Serial.println(depth);
  if(depth < 400){ //if the depth is below the minimum threshold
    //too shallow: low tone on buzzer
    tone(buzzerPin, 200);
    Serial.println("Shallow");
  }else if(depth > maxDepth){
    //too deep: high tone on buzzer
    tone(buzzerPin, 400);
    //Serial.println("High depth"); - for debugging
  }else{
    noTone(buzzerPin);
    //Serial.println("Good depth"); - for debugging
  }
}

void setLEDBrightness(int level) {//function to set brightness based on difficulty
  switch(level){ //another switch-case block
    case 0: //difficulty 0
      analogWrite(difficultyGreenLED, 255); // Full brightness
      break;
    case 1: //difficulty 1
      analogWrite(difficultyGreenLED, 128); // Half brightness
      break;
    case 2: //difficulty 2
      analogWrite(difficultyGreenLED, 64);  // Quarter brightness
      break;
  }
}

