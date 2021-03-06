///// test code for gibbon-II
#include <Thread.h>
#include <ThreadController.h>

const int totalSensors = 6;
int sensors[totalSensors];
int sensorPins [totalSensors] = {2, 4, 7, 8, 12, 13};
int motorPins [totalSensors] = {3, 5, 6, 9, 10, 11};
float initRiseTime, initResetTime, initShyTime, durShyTime, resetShyTime;
bool riseTrigger = false;
bool resetTrigger = false;
bool shyTrigger = false;
float riseLimit = 30000; // working time
float resetLimit = 10000; // rest time
float shyRunLimit = 10000;
float shyResetLimit = 5000;
boolean resetState = false;
boolean stopState [totalSensors];

// ThreadController that will controll all threads
ThreadController controll = ThreadController();

Thread* servosThread = new Thread();
Thread sensorsThread = Thread();

void servosCallback() {
  sweep();
}

void sensorsCallback() {
  checkSensors();
}

void setup() {
  Serial.begin(115200);
  delay(100);//wait for baudrate setting finished
  for (int i = 0; i < totalSensors; i++) {
    pinMode(sensorPins[i], INPUT);
    pinMode(motorPins[i], OUTPUT);
    stopState[i] = false;
  }

  servosThread->onRun(servosCallback);
  servosThread->setInterval(1600);

  // Configure myThread
  sensorsThread.onRun(sensorsCallback);
  sensorsThread.setInterval(100);

  // Adds both threads to the controller
  controll.add(servosThread);
  controll.add(&sensorsThread); // & to pass the pointer to it

}
void loop() {
  controll.run();
  /// Running moment ////

  if (resetState == false) {
    if (riseTrigger == false) {
      initRiseTime = millis();
//      if (shyTrigger == true) {
//        initShyTime =  millis();
//      } else {
//        resetShyTime = millis();
//      }
    }
    riseTrigger = true;
    eventRiseTimer();
//    shyRunTimer();
//    shyResetTimer();

  } else {
    /// stop moment ////
    if (resetTrigger == false) {
      initResetTime = millis();
    }
    resetTrigger = true;
    eventResetTimer();

  }
  for (int i = 0; i < totalSensors; i++) {
    if (stopState[i]) {
      digitalWrite(motorPins[i], LOW);
    } else {
      digitalWrite(motorPins[i], HIGH);
    }

  }

}

void sweep() {
  String values [6] = {
    "#0 P900 S300 #1 P2100 S300",
    " #8 P900 S300 #9 P2100 S300",
    " #16 P900 S300 #17 P2100 S300",
    " #4 P700 S300 #5 P2300 S300",
    " #12 P700 S300 #13 P2300 S300",
    " #20 P700 S300 #21 P2300 S300"
  };

  for (int i = 0; i < totalSensors; i++) {
    if (sensors[i] == 1) {
      stopState[i] = true;
      shyTrigger = true;
      durShyTime = millis();
//      Serial.println(durShyTime);
      switch (sensorPins[i]) {
        case 2:
          values[i] = "#0 P700 S300 #1 P2300 S300";
          break;
        case 4:
          values[i] = " #8 P700 S300 #9 P2300 S300";
          break;
        case 7:
          values[i] = " #16 P700 S300 #17 P2300 S300";
          break;
        case 8:
          values[i] = " #4 P900 S300 #5 P2100 S300";
          break;
        case 12:
          values[i] = " #12 P900 S300 #13 P2100 S300";
          break;
        case 13:
          values[i] = " #20 P900 S300 #21 P2100 S300";
          break;
        default:
          values [0] = "#0 P700 S300 #1 P2300 S300";
          values [1] = " #8 P700 S300 #9 P2300 S300";
          values [2] = " #16 P700 S300 #17 P2300 S300";
          values [3] = " #4 P900 S300 #5 P2100 S300";
          values [4] = " #12 P900 S300 #13 P2100 S300";
          values [5] = " #20 P900 S300 #21 P2100 S300";
          break;

      };
    } else {
      stopState[i] = false;

      //      shyStopTimer();
    }
  }


  if (resetState == false) {
    String stopCom = values[0] + values[1] + values[2] + values[3] + values[4] + values[5];
    String runCom = "#0 P700 S300 #1 P2300 S300 #8 P700 S300 #9 P2300 S300 #16 P700 S300 #17 P2300 S300 #4 P900 S300 #5 P2100 S300 #12 P900 S300 #13 P2100 S300 #20 P900 S300 #21 P2100";
    Serial.print(stopCom);
//    delay(20);
    Serial.print("\r");
    Serial.print("#3 P800 S500 #7 P800 S500 #11 P820 S500 #15 P600 S500 #19 P650 S500 #23 P650 S500");
    Serial.print("\r");
    delay(1000);
    Serial.print(runCom);
//    delay(20);
    Serial.print("\r");
    Serial.print("#3 P990 S3000 #7 P1010 S3000 #11 P1040 S3000 #15 P960 S3000 #19 P990 S3000 #23 P980 S3000");
    Serial.print("\r");
    delay(5000);
  }
  else {
    String stopCom = "#0 P2150 S400 #1 P850 S400 #8 P2150 S400 #9 P850 S400 #16 P2200 S400 #17 P800 S400 #4 P1700 S250 #5 P1300 S250 #12 P1700 S250 #13 P1300 S250 #20 P1700 S250 #21 P1300 S250";
    Serial.print(stopCom);
    delay(20);
    Serial.print("\r");
    delay(800);
  }

}

int* checkSensors()  {
  for (int i = 0; i < totalSensors; i++) {
    sensors[i] = digitalRead(sensorPins[i]);
    Serial.print(sensors[i]);
    Serial.print(" / ");
  }
  Serial.println("");
  return sensors;
}

void eventRiseTimer() {
  if (millis() - initRiseTime > riseLimit) {
    resetState = true;
    riseTrigger = false;
    initRiseTime = millis();
  }
}

void eventResetTimer() {
  if (millis() - initResetTime > resetLimit) {
    resetState = false;
    resetTrigger = false;
    initResetTime = millis();
  }
}

void shyRunTimer() {
  if (initShyTime > 0); {
    if (durShyTime - initShyTime > shyRunLimit) {
      resetState = true;
      riseTrigger = false;
      shyTrigger = false;
      initShyTime = 0;
    }
  }
}

void shyResetTimer() {
  if (resetShyTime - durShyTime > shyResetLimit) {
    shyTrigger = false;
    resetState = false;
    resetTrigger = false;
    initShyTime = 0;
  }

}
