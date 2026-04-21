#include <ESP32Servo.h>

Servo myservo; 

// Configuration
static const int servoPin = 14;   // PWM pin for Servo
int switchPin = 15;              // Pin for the toggle switch
int pos = 0;                     // Variable to store the servo position

// Calibration - Adjust these values based on your box's physical limits
int servoPositionDown = 40;      // Resting position inside the box
int servoPositionUp = 140;       // Position required to flip the switch

int switchState = 0; 
int previousSwitchState = 0; 

void setup() {
  // Allow the power to stabilize
  delay(500);
  
  // ESP32Servo library setup
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // Standard 50hz servo
  
  myservo.attach(servoPin, 500, 2400); // Standard min/max pulses for MG996R
  
  pinMode(switchPin, INPUT_PULLUP); 
  
  // Start in the hidden position
  myservo.write(servoPositionDown); 
  Serial.begin(115200);
}

void loop() {
  // Read the state of the switch (LOW = Pressed/On because of INPUT_PULLUP)
  switchState = digitalRead(switchPin);

  // TRIGGER: Switch is turned ON
  if (switchState == LOW && previousSwitchState == HIGH) {
    delay(50); // Debounce
    
    Serial.println("Switch flipped! Reacting...");

    // Attack: Move to flip the switch
    // The MG996R is heavy, so we use a loop to control speed
    for (pos = servoPositionDown; pos <= servoPositionUp; pos += 2) {
      myservo.write(pos);
      delay(10); // Decrease this delay to make the arm "angry"/faster
    }
    
    // Brief pause at the top to ensure the click happens
    delay(100);
  }
  
  // RETREAT: Once the switch is back to HIGH (Off), go back inside
  if (switchState == HIGH && previousSwitchState == LOW) {
    Serial.println("Mission accomplished. Retracting.");
    
    for (pos = servoPositionUp; pos >= servoPositionDown; pos -= 2) {
      myservo.write(pos);
      delay(15); // Slower retreat looks more "satisfied"
    }
  }

  previousSwitchState = switchState;
}