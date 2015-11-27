const int totalSensors = 6;
int sensors[totalSensors];
int sensorPins [totalSensors] = {2, 3, 4, 5, 6, 7};

int motorPins [totalSensors] = { 8, 9, 10, 11, 12, 13};
void setup() {
  Serial.begin(9600);
  delay(100);//wait for baudrate setting finished
  for (int i = 0; i < totalSensors; i++) {
    pinMode(sensorPins[i], INPUT);
    pinMode(motorPins[i], OUTPUT);
  }
}
void loop() {
  for (int i = 0; i < totalSensors; i++) {
    sensors[i] = digitalRead(sensorPins[i]);
    Serial.print(sensors[i]);
    Serial.print(" / ");
    if (sensors[i] == 1) {
      digitalWrite(motorPins[i], LOW);
    }
    else {
      digitalWrite(motorPins[i], HIGH);
    }

  }
  Serial.println("");
}

