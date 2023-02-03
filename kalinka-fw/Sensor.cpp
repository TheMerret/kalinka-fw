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

  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(1500);
  stepper.setSpeed(5000);
}

float Sensor::captureDistance() {
  // TOD0: do only if all task done: scene rotated, scanners tasks done (raised, rotated), buffer not full
  DBGLN("sensor capture distance");
  float volts = analogRead(sensorPin) * 0.0048828125;
  return 13 * pow(volts, -1);
}

void Sensor::rotateVertically(float degree) {
  // TODO: do only if not rotatating right now
 servo_sensor_vertical.write(verticalAngle + degree);
  DBG("sensor 1 rotate vertically on ");
  DBGLN(degree);
  verticalAngle += degree;
}

void Sensor::rotateHorizontally(float degree) {
  // TODO: do only if not rotatating right now
  servo_sensor_horizontal.write(horizontalAngle + degree);
  DBG("sensor 1 rotate horizontally on ");
  DBGLN(degree);
  horizontalAngle += degree;
}

void Sensor::raise(float h) {
  // TODO: do only if not raising right now
  stepper.moveTo(h);
  stepper.run();
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
    stepper.moveTo(pos);
  }
  Serial.println(digitalRead(buttonPin));
  stepper.run();
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

byte *SensorPacket::toBytes() {
  byte raw[sizeof(SensorPacket)];
  memcpy(raw, &height, sizeof(height));
  memcpy(raw + sizeof(height), &horizontalAngle, sizeof(horizontalAngle));
  memcpy(raw + sizeof(height) + sizeof(horizontalAngle), &verticalAngle, sizeof(verticalAngle));
  memcpy(raw + sizeof(height) + sizeof(horizontalAngle) + sizeof(verticalAngle), &distance, sizeof(distance));
  return raw;
}
