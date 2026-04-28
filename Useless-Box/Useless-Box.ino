#include <ESP32Servo.h>

Servo myservo;

// Configuration
static const int servoPin = 14;
static const int switchPin = 15;

// Calibration
int servoPositionDown = 40;
int servoPositionMiddle = 70;
int servoPositionUp = 135;

// Volatile variables are required for interrupts
volatile bool switchChanged = false;
volatile int currentTargetState = HIGH;

void IRAM_ATTR handleSwitchInterrupt() {
  // Read the physical pin state
  currentTargetState = digitalRead(switchPin);
  // Signal the main loop that we need to change direction
  switchChanged = true;
}

void setup() {
  delay(500);
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  
  // Set up the switch with a Pull-Up
  pinMode(switchPin, INPUT_PULLUP);
  
  // Attach the interrupt to trigger on ANY change (Low to High or High to Low)
  attachInterrupt(digitalPinToInterrupt(switchPin), handleSwitchInterrupt, CHANGE);

  myservo.write(servoPositionDown);
  Serial.begin(115200);
  Serial.println("Interrupt System Initialized.");
}

// Function to move the servo with "Interrupt Awareness"
void moveServo(int start, int end, int stepDelay) {
  // Prevent jitter if we are already at or very close to the destination
  if (abs(start - end) <= 2) {
    myservo.write(end);
    return;
  }

  int step = (start < end) ? 2 : -2;
  
  for (int p = start; (step > 0 ? p <= end : p >= end); p += step) {
    // If the interrupt flag is set, we stop this movement immediately
    if (switchChanged) {
      switchChanged = false; // Reset the flag
      return; 
    }
    
    myservo.write(p);
    delay(stepDelay);
  }
  
  // Final set to target to ensure precision
  myservo.write(end);
}

void loop() {
  int currentState = digitalRead(switchPin);
  int currentPos = myservo.read();

  // If the switch is ON (HIGH)
  if (currentState == HIGH) {
    // Only move if we aren't already at the 'Up' position
    if (currentPos < servoPositionUp - 2) {
      Serial.println("Switch is ON - Attacking");
      moveServo(currentPos, servoPositionUp, 10);
    }
  } 
  // If the switch is OFF (LOW)
  else {
    // Only move if we aren't already at the 'Down' position
    if (currentPos > servoPositionDown + 2) {
      Serial.println("Switch is OFF - Retracting");
      moveServo(currentPos, servoPositionDown, 15);
    }
  }

  // Small delay to prevent the loop from hammering the CPU
  delay(20);
}