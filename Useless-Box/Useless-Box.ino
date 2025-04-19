// Useless Box Code
// ----------------
// This code controls a servo motor to flip a switch on a useless box.
//
// Connections:
// - Servo Motor: Connect the servo motor to a PWM pin on the Arduino (e.g., Pin 9).
// - Switch: Connect one side of the switch to a digital pin (e.g., Pin 2) and the other side to GND.
//
// Functionality:
// 1.  The box is turned "on" (simulated by the switch being closed).
// 2.  The Arduino detects the switch state.
// 3.  The servo moves to flip the switch.
// 4.  The servo returns to its initial position.
// 5.  The process repeats when the switch is turned "on" again.

#include <Servo.h> // Include the Servo library

Servo myservo;     // Create a Servo object
int switchPin = 2; // Define the pin for the switch
int servoPin = 9;  // Define the pin for the servo motor
int pos = 0;       // Initial position of the servo
int switchState = 0; // Variable to store the switch state
int previousSwitchState = 0; // Variable to store the previous switch state

void setup() {
  pinMode(switchPin, INPUT_PULLUP); // Set the switch pin as an input with pull-up resistor
  myservo.attach(servoPin);         // Attach the servo to the servo pin
  myservo.write(90);                 // Initialize the servo to the center position (adjust as needed)
  pos = 90;
  Serial.begin(9600);
}

void loop() {
  // Read the state of the switch.  The pull-up resistor means it reads HIGH when open, LOW when closed.
  switchState = digitalRead(switchPin);

  // Check if the switch has just been turned on (transition from HIGH to LOW)
  if (switchState == LOW && previousSwitchState == HIGH) {
    delay(100); // Small delay for debouncing

    // --- Useless Action Sequence ---
    // Move servo to press the switch
    for (pos = 90; pos <= 180; pos += 1) { // from 90 to 180 degrees
      myservo.write(pos);
      delay(10);
    }
    delay(500); // Keep the servo in position for a short time

    // Move servo back to the initial position
    for (pos = 180; pos >= 90; pos -= 1) {
      myservo.write(pos);
      delay(10);
    }
    delay(200);
    // ------------------------------
  }
  previousSwitchState = switchState; // Update the previous switch state
}
