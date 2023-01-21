#include <Arduino.h>

#include "Sensor.h"

float Sensor::captureDistance() {
  // TOD0: do only if all task done: scene rotated, scanners tasks done (raised, rotated), buffer not full
  DBGLN("sensor capture distance");
  float volts = analogRead(sensorPin) * 0.0048828125;
  return 13 * pow(volts, -1);
}

void Sensor::rotateVertically(float degree) {
  // TODO: do only if not rotatating right now  
  DBG("sensor 1 rotate vertically on ");
  DBGLN(degree);
  verticalAngle += degree;
}

void Sensor::rotateHorizontally(float degree) {
  // TODO: do only if not rotatating right now
  DBG("sensor 1 rotate horizontally on ");
  DBGLN(degree);
  horizontalAngle += degree;
}

void Sensor::raise(float h) {
  // TODO: do only if not raising right now
  DBG("sensor 1 raise on ");
  DBGLN(height);
  height += h;
}

void Sensor::resetHorizontalAngle() {
  // TODO: do only if not rotatating right now
  DBGLN("reset reset horizontal rotation");
  horizontalAngle = 0.0;
}

void Sensor::resetVerticalAngle() {
  // TODO: do only if not rotatating right now
  DBGLN("sensor reset vertical rotation");
  verticalAngle = 0.0;  // TODO: (scannig_direction & SCANNING_VERTICALLY) ? -MAX_SENSOR_VERTICAL_ROTATION : 0.0;
}

void Sensor::resetHeight() {
  // TODO: do only if not raising right now
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
