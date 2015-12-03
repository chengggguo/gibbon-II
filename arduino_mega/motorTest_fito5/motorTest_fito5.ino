#include <Servo.h>

const int totalElements = 6;
int motorPins [totalElements] = {31, 47, 33, 45, 35, 43};
int sensorPins [totalElements] = {14, 15, 16, 17, 18, 19};

Servo bubbleServo;
int bubbleServoPin = 44;
int bubbleServoAngleLimit = 60;
int bubbleServoAngleStart = 30;
int openSpeed = 30;
int closeSpeed = 3;
int timeOpen = 0; //time bubbles open

const int totalServos = 12;
Servo servos [totalServos];
int servoPins [totalServos] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int servoPos = 0;
int servoInitPos = 20;
int angleLimit = 170;
int sweepLimit = 35;
int angleLimitShyState = 105;
int directionCorrect = 90;
int teethCorrect = 5;
int shortArmStopShyAngle = 105;
bool armState[totalElements];
int stateIndex = 0;
int isItShyCounter = 0;
int shyCycleLimit = 50; // default use 50 // measured in the system's cycles --> about 1 second each cycle

bool ison = true;
int generalCycle = 0;
int generalLoopLimit = 10; // default use 90 // measured in the system's cycles --> about 1 second each cycle
unsigned long halt = 4800;// for 8 mins use 480000

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
  if (ison) {
    defaultState();
    if (isItShyCounter < shyCycleLimit) {
      sweepAll(8);
    } else {
      runShyState(6);
      delay(halt);//8 mins delay TOTAL HALT OF THE SYSTEM
      transitionToStart(10);
      generalCycle = 0;
      isItShyCounter = 0;
      delay(1000);
    }
    Serial.print("Shy-count: ");
    Serial.println(isItShyCounter);
    generalCycle++;
    Serial.print("General-count: ");
    Serial.println(generalCycle);
    if (generalCycle >= generalLoopLimit) {
      ison = false;
    }
  } else {
    runShyState(6);
    delay(halt);//8 mins delay TOTAL HALT OF THE SYSTEM
    transitionToStart(10);
    ison = true;
    generalCycle = 0;
    isItShyCounter = 0;
    delay(1000);
  }
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
  float mapped;
  int shyCountTrigger = 0;
  for (servoPos = servoInitPos; servoPos <= sweepLimit; servoPos += 1) {
    stateIndex = 0;
    for (int i = 0; i <= totalServos - 1; i += 2) {
      if (armState[stateIndex] == false) {
        servos[i + 1].write(servoPos);
        servos[i].write(angleLimit - servoPos);
        delay(inc);
      } else {
        if (shyCountTrigger == 0) {
          isItShyCounter++;
        }
        shyCountTrigger++;
      }
      stateIndex++;
    }
    //mapped = map(servoPos, servoInitPos, sweepLimit, bubbleServoAngleStart, bubbleServoAngleLimit);
  }
  for (int bs = bubbleServoAngleStart; bs < bubbleServoAngleLimit; bs++) {
    bubbleServo.write(bs);
    delay(closeSpeed);
  }

  for (servoPos = sweepLimit; servoPos >= servoInitPos; servoPos -= 1) {
    stateIndex = 0;
    for (int i = 0; i <= totalServos - 1; i += 2) {
      //Serial.println(armState[stateIndex]);
      if (armState[stateIndex] == false) {
        servos[i + 1].write(servoPos);
        servos[i].write(angleLimit - servoPos);
        delay(inc);
      } else {
        if (shyCountTrigger == 0) {
          isItShyCounter++;
        }
        shyCountTrigger++;
      }
      stateIndex++;
    }
    //mapped = map(servoPos, sweepLimit, servoInitPos, bubbleServoAngleLimit, bubbleServoAngleStart);
  }
  for (int bs = bubbleServoAngleLimit; bs > bubbleServoAngleStart; bs--) {
    bubbleServo.write(bs);
    delay(openSpeed);
  }
  delay(timeOpen);
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
void runShyState(int inc) {
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
        if (servoPos < shortArmStopShyAngle) {
          servos[i + 1].write(servoPos + teethCorrect);
          servos[i].write(angleLimit - servoPos + teethCorrect);
        }
        delay(inc);
      }
    }
  }
}

//----------------------------------
void transitionToStart(int slowInc) {
  for (servoPos = angleLimitShyState; servoPos >= servoInitPos; servoPos -= 1) {
    for (int i = 0; i <= totalServos - 1; i += 2) {
      if (servoPins[i] == 2 || servoPins[i] == 4 || servoPins[i] == 6) {
        servos[i + 1].write(servoPos );
        servos[i].write(angleLimit - servoPos);
        delay(slowInc);
      } else {
        if (servoPos < shortArmStopShyAngle) {
          servos[i + 1].write(servoPos + teethCorrect);
          servos[i].write(angleLimit - servoPos + teethCorrect);
        }
        delay(slowInc);
      }
    }
    bubbleServo.write(servoPos);
    delay(slowInc);
  }
}

