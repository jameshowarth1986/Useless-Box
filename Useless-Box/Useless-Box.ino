#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
static const int servoPin = 14;
int switchPin = 2; // Define the pin for the switch
int switchState = 0; // Variable to store the switch state
int previousSwitchState = 0; // Variable to store the previous switch state

void setup() {
  myservo.attach(servoPin);  // attaches the servo on pin 9
  pinMode(switchPin, INPUT_PULLUP); // Set the switch pin as an input with pull-up resistor
  myservo.write(90); // Initialize the servo to the center position (adjust as needed) 
}

void loop() {
  // goes from 0 degrees to 180 degrees
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(30);               // waits 15ms for the servo to reach the position
  }
  
  // goes from 180 degrees to 0 degrees
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(15);               // waits 15ms for the servo to reach the position
  }
}