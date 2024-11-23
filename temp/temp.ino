int buttonPin = 9; //arbitrary pin for button
int potentiometer = 11; //arbitrary pin for potentiometer
int currentButtonState = 0; //variables to store button state (on or off)
int lastButtonState = 0;

unsigned long lastTime = 0; //stores last time in ms
int presses = 0; //stores number of button presses
float cpm = 0; //rate of presses per minute

void setup() {
  // put your setup code here, to run once:
  pinMode(button, INPUT_PULLUP); //setting button pin to input
  pinMode(potentiometer, INPUT_PULLUP); //setting potentiometer pin to input
  Serial.begin(9600)
}

float getDepth(){ //returns a value for depth as a value between 0-100
  reading = analogRead(potentiometer);
  return (reading/1023)*100;
}

void loop() {
  buttonState = digitalRead(buttonPin); //get button state

  if (buttonState == LOW && lastButtonState == HIGH) {//check if button is pressed
    pressCount++;  // Increment the press count
  }

  lastButtonState = buttonState; // Update last button state
  
  if (millis() - lastTime >= 1000) {
    pressesPerMinute = (pressCount * 60.0);  // Convert to per minute rate
    pressCount = 0;                          // Reset count for next interval
    lastTime = millis();                     // Reset time for the next interval

    // Print the rate to the serial monitor
    Serial.print("Presses per minute: ");
    Serial.println(pressesPerMinute);
  }
}

