#include <Arduino.h>

#include "Sensor.h"

float Sensor::captureDistance() {
  Serial.println("sensor capture distance");
  float volts = analogRead(sensorPin) * 0.0048828125;
  return 13 * pow(volts, -1);
}

void Sensor::rotateVertically(float degree) {
  Serial.print("sensor 1 rotate vertically on ");
  Serial.println(degree);
  verticalAngle += degree;
}

void Sensor::rotateHorizontally(float degree) {
  Serial.print("sensor 1 rotate horizontally on ");
  Serial.println(degree);
  horizontalAngle += degree;
}

void Sensor::raise(float h) {
  Serial.print("sensor 1 raise on ");
  Serial.println(height);
  height += h;
}

void Sensor::resetHorizontalAngle() {
  Serial.println("reset reset horizontal rotation");
  horizontalAngle = 0.0;
}

void Sensor::resetVerticalAngle() {
  Serial.println("sensor reset vertical rotation");
  verticalAngle = 0.0;  // TODO: (scannig_direction & SCANNING_VERTICALLY) ? -MAX_SENSOR_VERTICAL_ROTATION : 0.0;
}

void Sensor::resetHeight() {
  Serial.println("sensor reset height");
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
