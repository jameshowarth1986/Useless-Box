#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
static const int servoPin = 14;
int switchPin = 15; // Define the pin for the switch
int switchState = 0; // Variable to store the switch state
int previousSwitchState = 0; // Variable to store the previous switch state
int servoPositionDown = 40;
int servoPositionUp = 140;

void setup() {
  myservo.attach(servoPin);  // attaches the servo on pin 9
  pinMode(switchPin, INPUT_PULLUP); // Set the switch pin as an input with pull-up resistor
  myservo.write(servoPositionDown); // Initialize the servo to the center position (adjust as needed) 
}

void loop() {
  // Read the state of the switch.  The pull-up resistor means it reads HIGH when open, LOW when closed.
  switchState = digitalRead(switchPin);

  if (switchState == LOW && previousSwitchState == HIGH) {
    delay(100); // Small delay for debouncing
    // goes from down position to up degrees
    for (pos = servoPositionDown; pos <= servoPositionUp; pos += 1) {
      myservo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(10);               // waits 15ms for the servo to reach the position
    }
  }
  
  if (switchState == HIGH && previousSwitchState == LOW) {
    // goes from 180 degrees to 0 degrees
    for (pos = servoPositionUp; pos >= servoPositionDown; pos -= 1) {
      myservo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(15);               // waits 15ms for the servo to reach the position
    }
  }
  previousSwitchState = switchState; // Update the previous switch state
}