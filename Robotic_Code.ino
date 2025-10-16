/*
  Arduino Robotic Arm Controlled via Bluetooth
  Original version rewritten by Dayeli Matos
*/

#include <Servo.h>
#include <SoftwareSerial.h>

// Servo objects for each joint of the arm
Servo joint1;
Servo joint2;
Servo joint3;
Servo joint4;
Servo joint5;
Servo joint6;

// Bluetooth communication (Arduino UNO pins 3 = RX, 4 = TX)
SoftwareSerial BT(3, 4);

// Current and previous servo angles
int angle1, angle2, angle3, angle4, angle5, angle6;
int prevAngle1, prevAngle2, prevAngle3, prevAngle4, prevAngle5, prevAngle6;

// Arrays to store recorded steps for automatic playback
int stepsJoint1[50], stepsJoint2[50], stepsJoint3[50], stepsJoint4[50], stepsJoint5[50], stepsJoint6[50];

int servoSpeed = 20;      // Default speed for servo movement
int stepIndex = 0;        // Current step index
String btData = "";       // Data received from Bluetooth

void setup() {
  // Attach servo objects to PWM pins on Arduino UNO
  joint1.attach(5);
  joint2.attach(6);
  joint3.attach(7);
  joint4.attach(8);
  joint5.attach(9);
  joint6.attach(10);

  // Initialize Bluetooth communication
  BT.begin(38400);
  BT.setTimeout(1);
  delay(20);

  // Set initial positions for the robotic arm
  prevAngle1 = 90; joint1.write(prevAngle1);
  prevAngle2 = 150; joint2.write(prevAngle2);
  prevAngle3 = 35; joint3.write(prevAngle3);
  prevAngle4 = 140; joint4.write(prevAngle4);
  prevAngle5 = 85; joint5.write(prevAngle5);
  prevAngle6 = 80; joint6.write(prevAngle6);
}

void loop() {
  // Check for incoming Bluetooth commands
  if (BT.available() > 0) {
    btData = BT.readString();

    // Servo movement commands: "j1" to "j6"
    handleServoCommand(btData);

    // Save current positions to step arrays
    if (btData.startsWith("SAVE")) {
      saveStep();
    }

    // Run automatic playback of saved steps
    if (btData.startsWith("RUN")) {
      playbackSteps();
    }

    // Reset saved steps
    if (btData == "RESET") {
      resetSteps();
    }
  }
}

// Function to handle single servo movement commands
void handleServoCommand(String command) {
  int targetAngle;

  // Joint 1
  if (command.startsWith("j1")) {
    targetAngle = command.substring(2).toInt();
    moveServoSmooth(joint1, prevAngle1, targetAngle, 20);
    prevAngle1 = targetAngle;
  }
  // Joint 2
  else if (command.startsWith("j2")) {
    targetAngle = command.substring(2).toInt();
    moveServoSmooth(joint2, prevAngle2, targetAngle, 50);
    prevAngle2 = targetAngle;
  }
  // Joint 3
  else if (command.startsWith("j3")) {
    targetAngle = command.substring(2).toInt();
    moveServoSmooth(joint3, prevAngle3, targetAngle, 30);
    prevAngle3 = targetAngle;
  }
  // Joint 4
  else if (command.startsWith("j4")) {
    targetAngle = command.substring(2).toInt();
    moveServoSmooth(joint4, prevAngle4, targetAngle, 30);
    prevAngle4 = targetAngle;
  }
  // Joint 5
  else if (command.startsWith("j5")) {
    targetAngle = command.substring(2).toInt();
    moveServoSmooth(joint5, prevAngle5, targetAngle, 30);
    prevAngle5 = targetAngle;
  }
  // Joint 6
  else if (command.startsWith("j6")) {
    targetAngle = command.substring(2).toInt();
    moveServoSmooth(joint6, prevAngle6, targetAngle, 30);
    prevAngle6 = targetAngle;
  }
}

// Smoothly move a servo from previous to target angle
void moveServoSmooth(Servo &servo, int fromAngle, int toAngle, int delayMs) {
  if (fromAngle < toAngle) {
    for (int a = fromAngle; a <= toAngle; a++) {
      servo.write(a);
      delay(delayMs);
    }
  } else if (fromAngle > toAngle) {
    for (int a = fromAngle; a >= toAngle; a--) {
      servo.write(a);
      delay(delayMs);
    }
  }
}

// Save current positions into step arrays
void saveStep() {
  stepsJoint1[stepIndex] = prevAngle1;
  stepsJoint2[stepIndex] = prevAngle2;
  stepsJoint3[stepIndex] = prevAngle3;
  stepsJoint4[stepIndex] = prevAngle4;
  stepsJoint5[stepIndex] = prevAngle5;
  stepsJoint6[stepIndex] = prevAngle6;
  stepIndex++;
}

// Reset all saved steps
void resetSteps() {
  memset(stepsJoint1, 0, sizeof(stepsJoint1));
  memset(stepsJoint2, 0, sizeof(stepsJoint2));
  memset(stepsJoint3, 0, sizeof(stepsJoint3));
  memset(stepsJoint4, 0, sizeof(stepsJoint4));
  memset(stepsJoint5, 0, sizeof(stepsJoint5));
  memset(stepsJoint6, 0, sizeof(stepsJoint6));
  stepIndex = 0;
}

// Playback the saved steps automatically
void playbackSteps() {
  while (btData != "RESET") {
    for (int i = 0; i <= stepIndex - 2; i++) {

      // Check for incoming commands during playback
      if (BT.available() > 0) {
        btData = BT.readString();
        if (btData == "PAUSE") {
          while (btData != "RUN") {
            if (BT.available() > 0) {
              btData = BT.readString();
              if (btData == "RESET") return;
            }
          }
        }
        // Adjust speed dynamically
        if (btData.startsWith("ss")) {
          servoSpeed = btData.substring(2).toInt();
        }
      }

      moveServoSmooth(joint1, stepsJoint1[i], stepsJoint1[i + 1], servoSpeed);
      moveServoSmooth(joint2, stepsJoint2[i], stepsJoint2[i + 1], servoSpeed);
      moveServoSmooth(joint3, stepsJoint3[i], stepsJoint3[i + 1], servoSpeed);
      moveServoSmooth(joint4, stepsJoint4[i], stepsJoint4[i + 1], servoSpeed);
      moveServoSmooth(joint5, stepsJoint5[i], stepsJoint5[i + 1], servoSpeed);
      moveServoSmooth(joint6, stepsJoint6[i], stepsJoint6[i + 1], servoSpeed);
    }
  }
}
