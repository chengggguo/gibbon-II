int sweepCount = 0;
int defaultSweepLimit = 20;

int collapseCount = 0;
int defaultCollapseLimit = 20;

int shyLimit = 3;
bool isShy = false;

const int totalSensors = 6;
int sensors[totalSensors];
int sensorPins [totalSensors] = {2, 4, 7, 8, 12, 13};
boolean stopState [totalSensors];

int motorPins [totalSensors] = {3, 5, 6, 9, 10, 11};

int sensorCount = 0;

typedef void (*GeneralSweep) ();
void defaultSweep (GeneralSweep callback); // prototype
typedef void (*SensorRead) ();
void defaultSensorRead (SensorRead callback); // prototype

String values [6] = {
  "#0 P900 S300 #1 P2100 S300",
  " #8 P900 S300 #9 P2100 S300",
  " #16 P900 S300 #17 P2100 S300",
  " #4 P700 S300 #5 P2300 S300",
  " #12 P700 S300 #13 P2300 S300",
  " #20 P700 S300 #21 P2300 S300"
};

void setup() {
  Serial.begin(115200);
  delay(100);//wait for baudrate setting finished
  for (int i = 0; i < totalSensors; i++) {
    pinMode(sensorPins[i], INPUT);
    pinMode(motorPins[i], OUTPUT);
    stopState[i] = false;
  }
}

void loop() {
  if (!isShy) {
    defaultSweep(collapse);
    defaultSensorRead(shy);
  }else{
    collapse();
  }
}

///////////////////////
void defaultSweep (GeneralSweep callback) {
  if (sweepCount > defaultSweepLimit) {
    callback();
  } else {
    String stopCom = values[0] + values[1] + values[2] + values[3] + values[4] + values[5];
    String runCom = "#0 P700 S300 #1 P2300 S300 #8 P700 S300 #9 P2300 S300 #16 P700 S300 #17 P2300 S300 #4 P900 S300 #5 P2100 S300 #12 P900 S300 #13 P2100 S300 #20 P900 S300 #21 P2100";
    Serial.print(stopCom);
    Serial.print("\r");
    Serial.print("#3 P1130 S3500 #15 P1130 S3500 #11 P1130 S3500 #7 P1130 S3500 #19 P1130 S3500 #23 P1130 S3500");
    Serial.print("\r");
    delay(800);
    Serial.print(runCom);
    Serial.print("\r");
    Serial.print("#3 P650 S1000 #15 P650 S1000 #11 P650 S1000 #7 P650 S1000 #19 P650 S1000 #23 P650 S1000");
    Serial.print("\r");
    delay(3000);
    sweepCount++;
    collapseCount = 0;
  }
}

///////////////////////
void collapse() {
  String stopCom = "#0 P2000 S400 #1 P1000 S400 #8 P2000 S400 #9 P1000 S400 #16 P2000 S400 #17 P1000 S400 #4 P1700 S250 #5 P1300 S250 #12 P1700 S250 #13 P1300 S250 #20 P1700 S250 #21 P1300 S250";
  Serial.print(stopCom);
  Serial.print("\r");
  delay(1000);
  collapseCount ++;
  if (collapseCount > defaultCollapseLimit) {
    sweepCount = 0;
    isShy = false;
  }
}

///////////////////////
void shy() {
  sensorCount++;
  if (sensorCount > shyLimit) {
    isShy = true;
  }
}

///////////////////////
void defaultSensorRead (SensorRead callback) {
  values [0] = "#0 P900 S300 #1 P2100 S300";
  values [1] = " #8 P900 S300 #9 P2100 S300";
  values [2] = " #16 P900 S300 #17 P2100 S300";
  values [3] = " #4 P700 S300 #5 P2300 S300";
  values [4] = " #12 P700 S300 #13 P2300 S300";
  values [5] = " #20 P700 S300 #21 P2300 S300";
  for (int i = 0; i < totalSensors; i++) {
    sensors[i] = digitalRead(sensorPins[i]);
    if (sensors[i] == 1) {
      callback();
      digitalWrite(motorPins[i], LOW);
      stopState[i] = true;
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
      digitalWrite(motorPins[i], HIGH);
      stopState[i] = false;
    }
  }
}


