#include <Servo.h>

const int totalElements = 6;
int motorPins [totalElements] = {14, 15, 16, 17, 18, 19};
int sensorPins [totalElements] = {32, 34, 36, 38, 40, 42};

//BUBBLE SERVOS
Servo bubbleServo;
int bubbleServoPin = 44;
int bubbleServoAngleLimit = 60;
int bubbleServoAngleStart = 33;
int openSpeed = 40; //The higher this number the slower the reaction
int closeSpeed = 3; //The higher this number the slower the reaction
int timeOpen = 3000; //time bubbles open //The higher this number the slower the reaction

//ARMS
const int totalServos = 12;
Servo servos [totalServos];
int servoPins [totalServos] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int servoPos = 0;
int servoInitPos = 20;
int angleLimit = 170;
int sweepLimit = 35;
int angleLimitShyState = 110; //FOR LONG ARMS ONLY!!
int directionCorrect = 90;
int teethCorrect = 5;
int shortArmStopShyAngle = 90; //FOR SHORT ARMS ONLY!!
bool armState[totalElements];
int stateIndex = 0;
int isItShyCounter = 0;
int shyCycleLimit = 50; // default use 50 // measured in the system's cycles --> about 1 second each cycle
int temp = 0;
int s = 8;

//GENERAL CYCLE STUFF
bool ison = true;
int generalCycle = 0;
int generalLoopLimit = 70; // default use 90 // measured in the system's cycles --> about 1 second each cycle
unsigned long halt1 = 900000;// first delay when arms are UP for ---- 8 mins use 480000
unsigned long halt2 = 5000;// second delay when arms are DOWN for ---- 8 mins use 480000
int delayBeforeCollapse = 15000;
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
      runShyState(5);
      delay(halt1);//8 mins delay TOTAL HALT OF THE SYSTEM
      transitionToStart(10);
      delay(halt2);//8 mins delay TOTAL HALT OF THE SYSTEM
      generalCycle = 0;
      isItShyCounter = 0;
      delay(1000);
    }
    Serial.print("Shy-count: ");
    Serial.println(isItShyCounter);
    generalCycle++;
    Serial.print("General-count: ");
    Serial.println(generalCycle);
    Serial.print("Temp: ");
    Serial.println(temp);
    if (generalCycle >= generalLoopLimit) {
      ison = false;
    }
  } else {
    runShyState(6);
    delay(halt1);//8 mins delay TOTAL HALT OF THE SYSTEM
    transitionToStart(10);
    delay(halt2);//8 mins delay TOTAL HALT OF THE SYSTEM
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
  temp = 0;
  int shyCountTrigger = 0;
  for (servoPos = servoInitPos; servoPos <= sweepLimit; servoPos += 1) {
    stateIndex = 0;
    for (int i = 0; i <= totalServos - 1; i += 2) {
      if (armState[stateIndex] == false) {
        if (servoPins[i] == 2 || servoPins[i] == 4 || servoPins[i] == 6) {
          servos[i + 1].write(servoPos);
          servos[i].write(angleLimit - servoPos);
          delay(s);
        } else {
          servos[i].write(servoPos + directionCorrect + sweepLimit + teethCorrect);
          servos[i + 1].write(angleLimit - servoPos - directionCorrect - sweepLimit + teethCorrect);
          delay(s);
        }
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

  delay(1000);// close delay

  for (servoPos = sweepLimit; servoPos >= servoInitPos; servoPos -= 1) {
    stateIndex = 0;
    for (int i = 0; i <= totalServos - 1; i += 2) {
      if (armState[stateIndex] == false) {
        if (servoPins[i] == 2 || servoPins[i] == 4 || servoPins[i] == 6) {
          servos[i + 1].write(servoPos);
          servos[i].write(angleLimit - servoPos);
          delay(s);
        } else {
          servos[i].write(servoPos + directionCorrect + sweepLimit + teethCorrect);
          servos[i + 1].write(angleLimit - servoPos - directionCorrect - sweepLimit + teethCorrect);
          delay(s);
        }
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

  for (int t = 0; t < 6; t++) {
    temp += int(armState[t]);
  }

  if (temp >= 3) {
    s = inc * 2;
  } else {
    s = inc;
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
  delay(delayBeforeCollapse);
  for (servoPos = servoInitPos; servoPos <= angleLimitShyState; servoPos += 1) {
    for (int i = 0; i <= totalServos - 1; i += 2) {
      if (servoPins[i] == 2 || servoPins[i] == 4 || servoPins[i] == 6) {
        servos[i + 1].write(servoPos);
        servos[i].write(angleLimit - servoPos);
        delay(inc);
      }
      //      else {
      //        if (servoPos < shortArmStopShyAngle) {
      //          servos[i + 1].write(servoPos + teethCorrect);
      //          servos[i].write(angleLimit - servoPos + teethCorrect);
      //        }
      //        delay(inc);
      //      }
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
      }
      //      else {
      //        if (servoPos < shortArmStopShyAngle) {
      //          servos[i + 1].write(servoPos + teethCorrect);
      //          servos[i].write(angleLimit - servoPos + teethCorrect);
      //        }
      //        delay(slowInc);
      //      }
    }
    bubbleServo.write(servoPos);
    delay(slowInc);
  }
}

