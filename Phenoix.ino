#include<Servo.h>

// PHOENIX - Self Reconfiguring Robotic Car

Servo backupServo;

// Motor driver pins
const int pwmPins[4] = {3, 5, 6, 9};

const int in1Pins[4] = {
  2, 7, 10, 12
};

const int in2Pins[4] = {
  4, 8, 11, 13
};

// Wheel sensors
const int hallPins[4] = {
  A1, A2, A3, A4
};

const char* wheelNames[4] = {
  "Front Left",
  "Front Right",
  "Rear Left",
  "Rear Right"
};

bool failed[4] = {false, false, false, false};
bool previousState[4] = {false, false, false, false};

unsigned long lastPulse[4];
const unsigned long FAILURE_TIME = 1200;

const int MOTOR_SPEED = 130;

bool backupActivated = false;

void setup() {

  Serial.begin(9600);

  // Configure motors
  for (int i = 0; i < 4; i++) {

    pinMode(pwmPins[i], OUTPUT);
    pinMode(in1Pins[i], OUTPUT);
    pinMode(in2Pins[i], OUTPUT);

    pinMode(hallPins[i], INPUT_PULLUP);

    lastPulse[i] = millis();
  }

  // Backup mechanism
  backupServo.attach(A0);
  backupServo.write(0);

  delay(1000);

  Serial.println("PHOENIX SYSTEM STARTED");

  // Start all four wheels
  for (int i = 0; i < 4; i++) {
    moveWheel(i);
  }
}

void moveWheel(int i) {

  if (failed[i]) return;

  digitalWrite(in1Pins[i], HIGH);
  digitalWrite(in2Pins[i], LOW);

  analogWrite(pwmPins[i], MOTOR_SPEED);
}

void stopWheel(int i) {

  analogWrite(pwmPins[i], 0);

  digitalWrite(in1Pins[i], LOW);
  digitalWrite(in2Pins[i], LOW);
}

void detectFailure() {

  for (int i = 0; i < 4; i++) {

    bool currentState = digitalRead(hallPins[i]);

    // Detect sensor pulse
    if (currentState == HIGH &&
        previousState[i] == LOW) {

      lastPulse[i] = millis();
    }

    previousState[i] = currentState;

    // Check for wheel failure
    if (!failed[i] &&
        millis() - lastPulse[i] > FAILURE_TIME) {

      failed[i] = true;

      stopWheel(i);

      Serial.print("WHEEL FAILURE: ");
      Serial.println(wheelNames[i]);

      activateBackup();
    }
  }
}

void activateBackup() {

  if (backupActivated) return;

  backupActivated = true;

  Serial.println("ACTIVATING BACKUP MECHANISM");

  // Demonstration servo movement
  backupServo.write(90);

  delay(500);

  Serial.println("BACKUP SUPPORT DEPLOYED");
}

void loop() {

  detectFailure();

  // Keep functioning wheels moving
  for (int i = 0; i < 4; i++) {

    if (!failed[i]) {
      moveWheel(i);
    }
  }

  delay(10);
}