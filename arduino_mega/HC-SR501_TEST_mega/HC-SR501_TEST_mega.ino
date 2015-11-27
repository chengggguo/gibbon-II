const int totalSensors = 6;
int sensors[totalSensors];
int sensorPins [totalSensors] = {23, 25, 27, 29, 31, 33};

int motorPins [totalSensors] = { 22, 24, 26, 28, 30, 32};
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

