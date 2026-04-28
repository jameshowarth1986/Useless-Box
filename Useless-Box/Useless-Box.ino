#include <ESP32Servo.h>

Servo myservo;

// Configuration
static const int servoPin = 14;
static const int switchPin = 15;

// Calibration
int servoPositionDown = 40;
int servoPositionMiddle = 70;
int servoPositionUp = 135;

// --- PERSONALITY SYSTEM ---
// Structure: {ReactionDelay, AttackSpeed, RetreatSpeed}
const int NUM_PROFILES = 6;
int currentProfileIndex = 0;

const int profiles[NUM_PROFILES][3] = {
  {50, 3, 5},     // 0: The Aggressive (Instant reaction, super fast)
  {1500, 15, 25}, // 1: The Lazy (Waits a long time, moves slowly)
  {300, 8, 10},   // 2: The Normal (Standard behavior)
  {50, 60, 5},     // 3: The Sneaky (Starts moving immediately but slowly, then retreats fast)
  {800, 4, 60},   // 4: The Dramatic (Wait, strike fast, retreat very slowly)
  {200, 10, 15}   // 5: The Fake-Out (Logic handled separately in loop)
};

// Current active variability values
int reactionDelay = 0;
int attackSpeed = 10;
int retreatSpeed = 15;

// Volatile variables for interrupts
volatile bool switchChanged = false;
volatile int currentTargetState = HIGH;

void IRAM_ATTR handleSwitchInterrupt() {
  currentTargetState = digitalRead(switchPin);
  switchChanged = true;
}

// Function to pick a personality profile
void selectPersonality() {
  currentProfileIndex = random(0, NUM_PROFILES);
  
  reactionDelay = profiles[currentProfileIndex][0];
  attackSpeed   = profiles[currentProfileIndex][1];
  retreatSpeed  = profiles[currentProfileIndex][2];
  
  Serial.print("Selected Profile #");
  Serial.print(currentProfileIndex);
  if (currentProfileIndex == 5) Serial.print(" [FAKE-OUT]");
  Serial.print(" -> Delay: ");
  Serial.print(reactionDelay);
  Serial.print("ms, Attack: ");
  Serial.print(attackSpeed);
  Serial.print("ms, Retreat: ");
  Serial.println(retreatSpeed);
}

void setup() {
  delay(500);
  
  // Seed the random number generator
  randomSeed(analogRead(34)); 
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  
  pinMode(switchPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(switchPin), handleSwitchInterrupt, CHANGE);

  myservo.write(servoPositionDown);
  Serial.begin(115200);
  Serial.println("System Initialized with Fake-Out Logic.");
}

// Function to move the servo with "Interrupt Awareness"
bool moveServo(int start, int end, int stepDelay) {
  if (abs(start - end) <= 2) {
    myservo.write(end);
    return true;
  }

  int step = (start < end) ? 2 : -2;
  
  for (int p = start; (step > 0 ? p <= end : p >= end); p += step) {
    if (switchChanged) {
      switchChanged = false;
      return false; // Motion interrupted
    }
    
    myservo.write(p);
    delay(stepDelay);
  }
  
  myservo.write(end);
  return true; // Motion completed
}

// Helper to handle the reaction delay with interrupt check
bool waitWithInterrupt(int ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (switchChanged) {
      switchChanged = false;
      return false; // Interrupted during delay
    }
    delay(10);
  }
  return true;
}

void loop() {
  int currentState = digitalRead(switchPin);
  int currentPos = myservo.read();

  // If the switch is ON (HIGH)
  if (currentState == HIGH) {
    if (currentPos < servoPositionUp - 2) {
      selectPersonality();
      
      Serial.println("Switch is ON - Executing Profile...");
      
      // Standard initial delay for all profiles
      if (!waitWithInterrupt(reactionDelay)) return;

      // Special Logic for Profile 5 (The Fake-Out)
      if (currentProfileIndex == 5) {
        // 1. Move to 110
        if (!moveServo(currentPos, 110, attackSpeed)) return;
        
        // 2. Wait for 3 seconds
        if (!waitWithInterrupt(3000)) return;
        
        // 3. Move back to down position
        if (!moveServo(110, servoPositionDown, 15)) return;
        
        // 4. Wait for 1 second
        if (!waitWithInterrupt(1000)) return;
        
        // 5. Slowly move to the up position
        moveServo(servoPositionDown, servoPositionUp, 60); 
      } 
      else {
        // Standard profile behavior
        moveServo(currentPos, servoPositionUp, attackSpeed);
      }
    }
  } 
  // If the switch is OFF (LOW)
  else {
    if (currentPos > servoPositionDown + 2) {
      Serial.println("Switch is OFF - Retracting");
      moveServo(currentPos, servoPositionDown, retreatSpeed);
    }
  }

  delay(20);
}