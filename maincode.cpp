
#include <Servo.h> // Includes the library for controlling servo motors.
Servo servoLeft, servoRight; // Creates two Servo objects for the wheels.
const int centreLedPin = 6, centreReceiverPin = 7, centreRedLed = A1;
// Defines the pins for the LEFT sensor and its indicator LED.
const int leftLedPin = 10, leftReceiverPin = 11, leftRedLed = A2;
// Defines the pins for the RIGHT sensor and its indicator LED.
const int rightLedPin = 2, rightReceiverPin = 3, rightRedLed = A0;
unsigned long lastCornerMs; // Stores the time (in milliseconds) of the last 90-degree turn.
const unsigned long LONG_MS = 2500; // A 2.5-second timeout threshold.
int CCW_F = 1700; // Counter-Clockwise Fast
int CCW_S = 1600; // Counter-Clockwise Slow
int CW_F = 1300; // Clockwise Fast
int CW_S = 1400; // Clockwise Slow
int st = 1500; // Stop (neutral pulse)
int change;
void setup()
{
 // Attach the servo objects to their physical pins.
servoLeft.attach(13);
servoRight.attach(12);
 // Configure the sensor receiver pins as INPUTS.
pinMode(centreReceiverPin, INPUT);
pinMode(leftReceiverPin, INPUT);
pinMode(rightReceiverPin, INPUT);
 // Configure the sensor emitter and indicator LED pins as OUTPUTS.
pinMode(centreLedPin, OUTPUT);
pinMode(centreRedLed, OUTPUT);
pinMode(leftLedPin, OUTPUT);
pinMode(leftRedLed, OUTPUT);
pinMode(rightLedPin, OUTPUT);
pinMode(rightRedLed, OUTPUT);
 // Initialize serial communication for debugging (e.g., in Serial Monitor).
Serial.begin(9600);
 // Stop both motors to ensure the robot is still at the start.
servoLeft.writeMicroseconds(1500);
servoRight.writeMicroseconds(1500);
delay(300); // Wait for 300ms for systems to stabilize.
 // Initialize state variables.
 lastCornerMs = 0; // Set the last corner time to 0.
  change=0;
}
void loop()
{
int centreIrVal = irDetect(centreLedPin, centreReceiverPin, 38000);
int leftWallValue = distance(leftLedPin, leftReceiverPin);
int rightWallValue = distance(rightLedPin, rightReceiverPin);
int centreWallValue = distance(centreLedPin, centreReceiverPin);
if (leftWallValue < 9) {
digitalWrite(leftRedLed, HIGH); // Turn on LED if left wall is detected.
} else {
digitalWrite(leftRedLed, LOW);
}
if (rightWallValue < 9) {
digitalWrite(rightRedLed, HIGH); // Turn on LED if right wall is detected.
} else {
digitalWrite(rightRedLed, LOW);
}
if (centreWallValue < 9) {
digitalWrite(centreRedLed, HIGH); // Turn on LED if front wall is detected.
} else {
digitalWrite(centreRedLed, LOW);
}
Serial.println("------------");
Serial.print(leftWallValue);
Serial.print(centreWallValue);
Serial.println(rightWallValue);
 // Condition: Dead End (walls on all three sides).
if (centreWallValue < 9 && leftWallValue < 9 && rightWallValue < 9) {
 // Check how long it's been since the last 90-degree turn.
unsigned long elapsed = millis() - lastCornerMs;
 // If it's been a long time ( > 2.5s), we're in a long dead-end tunnel.
unsigned long programMs = millis();
if (elapsed > LONG_MS && programMs > 15000) {
while (true) stop(); // Stop permanently, we are trapped or finished.
}
 // Otherwise, it's a normal dead end.
else {
spin(); // Perform a 180-degree turn.
moveForward();
delay(400);
 lastCornerMs=millis();
 change=0;
return; // Restart the loop to get new sensor readings.
}
}
if (leftWallValue ==9 || rightWallValue ==9){
 lastCornerMs=millis();
}
if (leftWallValue <9 && rightWallValue<9){
unsigned long elapsed1 = millis() - lastCornerMs;
if (elapsed1>2000) {
   change=1;
}
}
if (change==1) {
Serial.println("new mode");
if (leftWallValue == 9) { // If the right side is open...
moveForward(); // ...move forward a little bit...
delay(340); // ...for 250ms (to clear the corner)...
turnLeft(); // ...then execute the 90-degree right turn.
moveForward(); // Resume moving forward.
delay(150);
 change=0;
return; // Restart the loop.
}
if (centreWallValue < 9 && rightWallValue == 9 && leftWallValue < 9) {
turnRight(); // ...then execute a 90-degree left turn.
moveForward(); // Resume moving forward.
delay(10);
 change=0;
return; // Restart the loop.
}
 // Condition: Wall in front, open on right (but blocked on left).
 // NOTE: This block is REDUNDANT. The main "RIGHT-HAND RULE"
if (centreWallValue < 9 && rightWallValue < 9 && leftWallValue == 9) {
moveForward();
delay(275);
turnLeft();
moveForward();
delay(10);
 change=0;
return;
}
}  
if (rightWallValue == 9) { // If the right side is open...
moveForward(); // ...move forward a little bit...
delay(470); // ...for 250ms (to clear the corner)...
turnRight(); // ...then execute the 90-degree right turn.
moveForward(); // Resume moving forward.
delay(170);
return; // Restart the loop.
}
if (centreWallValue < 9 && leftWallValue == 9 && rightWallValue < 9) {
moveForward();
delay(150);
turnLeft(); // ...then execute a 90-degree left turn.
moveForward(); // Resume moving forward.
delay(10);
return; // Restart the loop.
}
 // Condition: Wall in front, open on right (but blocked on left).
 // NOTE: This block is REDUNDANT. The main "RIGHT-HAND RULE"
if (centreWallValue < 9 && leftWallValue < 9 && rightWallValue == 9) {
moveForward();
delay(275);
turnRight();
moveForward();
delay(10);
return;
}
if (leftWallValue < rightWallValue + 3) {
if (leftWallValue < 6) {
adjustRight(); // ...make a slight adjustment to the right.
Serial.println("Right");
return; // Restart the loop.
}
}
 // Else if robot is closer to the right wall...
else if (rightWallValue + 3 < leftWallValue) {
 // ...and it's VERY close...
if (rightWallValue + 3< 6) {
adjustLeft(); // ...make a slight adjustment to the left.
Serial.println("Left");
return; // Restart the loop.
}
}
}
// Stops the robot.
void stop() {
servoLeft.writeMicroseconds(st); // Pulse near 1500 (neutral)
servoRight.writeMicroseconds(st); // Pulse near 1500 (neutral)
delay(1000); // Hold stop for 1 second.
}
// Moves the robot straight ahead.
void moveForward() {
servoLeft.writeMicroseconds(1670); // One motor forward
servoRight.writeMicroseconds(1300); // Other motor forward (opposite pulse for opposite
}
// Performs a ~90-degree left turn.
void turnLeft() {
servoLeft.writeMicroseconds(1495 - 15); // Left wheel stops/reverses slowly
servoRight.writeMicroseconds(1300); // Right wheel moves forward
delay(970); // Duration of the turn
 lastCornerMs = millis(); // *IMPORTANT:* Resets the dead-end timer.
moveForward(); // Resume moving forward.
}
// Performs a ~90-degree right turn.
void turnRight() {
servoLeft.writeMicroseconds(1600); // Left wheel moves forward
servoRight.writeMicroseconds(1490 + 15); // Right wheel stops/reverses slowly
delay(990); // Duration of the turn
 lastCornerMs = millis(); // *IMPORTANT:* Resets the dead-end timer.
moveForward(); // Resume moving forward.
}
// Spins the robot 180 degrees.
void spin() {
 // *POTENTIAL BUG:* Both motors are set to CCW_F (1700).
 // To spin, one should be forward (e.g., 1700) and the other
 // backward (e.g., 1300). This code will likely make the robot veer, not spin.
servoLeft.writeMicroseconds(CCW_F); // Left wheel
servoRight.writeMicroseconds(CCW_F); // Right wheel
delay(1020); // Duration of the "spin".
moveForward(); // Resume moving forward.
}
// Makes a slight adjustment to the right (veers right).
void adjustRight() {
servoLeft.writeMicroseconds(1690); // Left wheel slightly faster
servoRight.writeMicroseconds(1440); // Right wheel slightly slower
delay(50); // Adjust for only 25ms.
moveForward(); // Return to normal forward speed.
}
// Makes a slight adjustment to the left (veers left).
void adjustLeft() {
servoLeft.writeMicroseconds(1505); // Left wheel slightly slower
servoRight.writeMicroseconds(1300); // Right wheel (normal speed)
delay(50); // Adjust for only 25ms.
moveForward(); // Return to normal forward speed.
}
// Flashes an IR LED at a specific frequency and reads the receiver.
int irDetect(int irledPin, int receiverPin, int frequency) {
tone(irledPin, frequency); // Turn on IR LED at 'frequency'
delay(1);
noTone(irledPin); // Turn off IR LED
int ir = digitalRead(receiverPin); // Read the receiver
delay(1);
return ir; // Returns 0 if reflection is detected, 1 if not.
}
// Sweeps a range of frequencies to get a reliable "distance score".
int distance(int ledPin, int receiverPin) {
int distance = 0;
 // Loop from 38kHz to 42kHz
for (long f = 38000; f <= 42000; f += 500) {
 // Add the result (0 for detection, 1 for no detection)
 distance += irDetect(ledPin, receiverPin, f);
}
 // A low score (e.g., 0-3) means a wall is close.
 // A high score (e.g., 9) means no wall is detected.
return distance;}
