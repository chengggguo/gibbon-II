#include <Servo.h>

const int totalElements = 6;
int motorPins [totalElements] = {14, 15, 16, 17, 18, 19};
int sensorPins [totalElements] = {22, 23, 24, 25, 26, 27};

int bubbleServoPin = 44;
Servo bubbleServo;
const int totalServos = 12;
Servo servos [totalServos];
int servoPins [totalServos] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int servoPos = 0;
int servoInitPos = 10;
int angleLimit = 170;
int sweepLimit = 35;
int angleLimitShyState = 110;
bool armState[totalElements];
int stateIndex = 0;
int isItShyCounter = 0;

//----------------------------------
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < totalElements; i++) {
    pinMode(motorPins[i], OUTPUT);
    pinMode(sensorPins[i], INPUT);
    digitalWrite(motorPins[i], LOW);
    armState[i] = false;
  }
  for (int i = 0; i < totalServos; i++) {
    servos[i].attach(servoPins[i]);
  }
  bubbleServo.attach(bubbleServoPin);
  resetPos();
  delay(1000);
}

//----------------------------------
void loop() {
  defaultState();
  if (isItShyCounter < 200) {
    sweepAll(8);
  } else {
    runShyState(6, 15000);
    transitionToStart(10);
    delay(1000);
  }
  Serial.print("Shy-count: ");
  Serial.println(isItShyCounter);
}

//----------------------------------
void defaultState() {
  for (int i = 0; i < totalElements; i++) {
    int senseVal = digitalRead(sensorPins[i]);
    if (senseVal == 1) {
      digitalWrite(motorPins[i], LOW);
      armState[i] = true;
    } else {
      digitalWrite(motorPins[i], HIGH);
      armState[i] = false;
    }
    Serial.print(senseVal);
    Serial.print(" / ");
  }
  Serial.println("");
}

//----------------------------------
void sweepAll(int inc) {
  for (servoPos = servoInitPos; servoPos <= sweepLimit; servoPos += 1) {
    stateIndex = 0;
    for (int i = 0; i <= totalServos - 1; i += 2) {
      if (armState[stateIndex] == false) {
        if (servoPins[i] == 2 || servoPins[i] == 4 || servoPins[i] == 6) {
          servos[i + 1].write(servoPos);
          servos[i].write(angleLimit - servoPos);
          delay(inc);
        } else {
          servos[i].write(angleLimit - servoPos);
          servos[i + 1].write(angleLimit - (angleLimit - servoPos));
          delay(inc);
        }
      } else {
        isItShyCounter++;
      }
      stateIndex++;
    }
    bubbleServo.write(servoPos);
    delay(inc);
  }
  for (servoPos = sweepLimit; servoPos >= servoInitPos; servoPos -= 1) {
    stateIndex = 0;
    for (int i = 0; i <= totalServos - 1; i += 2) {
      //Serial.println(armState[stateIndex]);
      if (armState[stateIndex] == false) {
        if (servoPins[i] == 2 || servoPins[i] == 4 || servoPins[i] == 6) {
          servos[i + 1].write(servoPos);
          servos[i].write(angleLimit - servoPos);
          delay(inc);
        } else {
          servos[i].write(angleLimit - servoPos);
          servos[i + 1].write(angleLimit - (angleLimit - servoPos));
          delay(inc);
        }
      } else {
        isItShyCounter++;
      }
      stateIndex++;
    }
    bubbleServo.write(servoPos);
    delay(inc);
  }
}

//----------------------------------
void resetPos() {
  for (int i = 0; i <= totalServos; i += 2) {
    servos[i + 1].write(servoInitPos);
    servos[i].write(angleLimit);
    delay(15);
  }
}

//----------------------------------
void runShyState(int inc, int waiting) {
  for (int i = 0; i < totalElements; i++) {
    digitalWrite(motorPins[i], LOW);
  }
  delay(5000);
  for (servoPos = 10; servoPos <= angleLimitShyState; servoPos += 1) {
    for (int i = 0; i <= totalServos - 1; i += 2) {
      if (servoPins[i] == 2 || servoPins[i] == 4 || servoPins[i] == 6) {
        servos[i + 1].write(servoPos);
        servos[i].write(angleLimit - servoPos);
        delay(inc);
      } else {
        servos[i + 1].write(servoPos);
        servos[i].write(angleLimit - servoPos);
        delay(inc);
      }
    }
  }
  delay(waiting);
}

//----------------------------------
void transitionToStart(int slowInc) {
  for (servoPos = angleLimitShyState; servoPos >= servoInitPos; servoPos -= 1) {
    for (int i = 0; i <= totalServos - 1; i += 2) {
      servos[i + 1].write(servoPos);
      servos[i].write(angleLimit - servoPos);
      delay(slowInc);
    }
    bubbleServo.write(servoPos);
    delay(slowInc);
  }
  isItShyCounter = 0;
}

