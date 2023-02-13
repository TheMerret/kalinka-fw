#include <Arduino.h>

#include "Sensor.h"

void Sensor::attach() {
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  pinMode(buttonPin, INPUT_PULLUP);
  servo_sensor_horizontal.attach(horizontalPin);
  //servo_sensor_vertical.attach(verticalPin);

  //  stepper.setMaxSpeed(1000);
  //  stepper.setAcceleration(50);
  //  stepper.setSpeed(200);

  heightStepper.setMaxSpeed(10000);
  heightStepper.setAcceleration(1500);
  heightStepper.setSpeed(5000);
}

float Sensor::captureDistance() {
  // TOD0: do only if all task done: scene rotated, scanners tasks done (raised, rotated), buffer not full
  DBGLN("sensor capture distance");
  float volts = analogRead(sensorPin) * 0.0048828125;
  return 13 * pow(volts, -1);
}

void Sensor::rotateVertically(float degree) {
  // TODO: do only if not rotatating  norightw
  servo_sensor_vertical.write(verticalAngle + degree);
  DBG("sensor 1 rotate vertically on ");
  DBGLN(degree);
  verticalAngle += degree;
}

void Sensor::rotateHorizontally(float degree) {
  // TODO: do only if not rotatating right now
  servo_sensor_horizontal.write(horizontalAngle + degree + 90);
  DBG("sensor 1 rotate horizontally on ");
  DBGLN(degree);
  horizontalAngle += degree;
}

void Sensor::raise(float h) {
  // TODO: do only if not raising right now
  Serial.println(heightPinStep);
  heightStepper.moveTo(height + h);
  heightStepper.run();
  DBG("sensor 1 raise on ");
  DBGLN(height);
  height += h;
}

void Sensor::resetHorizontalAngle() {
  // TODO: do only if not rotatating right now
  servo_sensor_horizontal.write(90);
  DBGLN("reset reset horizontal rotation");
  horizontalAngle = 0.0;
}

void Sensor::resetVerticalAngle() {
  // TODO: do only if not rotatating right now
  servo_sensor_vertical.write(90);
  DBGLN("sensor reset vertical rotation");
  verticalAngle = 0.0;  // TODO: (scannig_direction & SCANNING_VERTICALLY) ? -MAX_SENSOR_VERTICAL_ROTATION : 0.0;
}

void Sensor::resetHeight() {
  // TODO: do only if not raising right now
  int pos = 0;
  Serial.println(digitalRead(buttonPin));
  if (digitalRead(buttonPin) && millis() % 300 == 0) {
    pos += 50;
    heightStepper.moveTo(pos);
    Serial.println();
    Serial.println("DONE");
    Serial.println();
  }
  Serial.println(digitalRead(buttonPin));
  heightStepper.run();
  DBGLN("sensor reset height");
  height = 0.0;
}

void Sensor::reset() {
  resetHorizontalAngle();
  resetVerticalAngle();
  resetHeight();
}

SensorPacket Sensor::read() {
  return SensorPacket(height, horizontalAngle, verticalAngle, captureDistance());
}

StaticJsonDocument<SENSOR_DOCUMENT_CAPACITY> SensorPacket::serialize() {
  /*
  TODO: min size is 48
  {
  "sceneAngle": 1.1,
  "height": 1.1,
  "horizontalAngle": 1.1,
  "verticalAngle": 1.1,
  "distance": 1.1
  }
  */
  doc["sceneAngle"] = sceneAngle;
  doc["height"] = height;
  doc["horizontalAngle"] = horizontalAngle;
  doc["verticalAngle"] = verticalAngle;
  doc["distance"] = distance;
  return doc;  
}
