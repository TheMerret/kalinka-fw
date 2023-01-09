#include <Arduino.h>

#include "Sensor.h"

void Sensor::rotate_vertically(float degree) {
  Serial.print("sensor 1 rotate vertically on ");
  Serial.println(degree);
  vertical_angle += degree;
}

void Sensor::rotate_horizontally(float degree) {
  Serial.print("sensor 1 rotate horizontally on ");
  Serial.println(degree);
  horizontal_angle += degree;
}

void Sensor::raise(float h) {
  Serial.print("sensor 1 raise on ");
  Serial.println(height);
  height += h;
}

void Sensor::reset_horizontal_angle() {
  Serial.println("reset reset horizontal rotation");
  horizontal_angle = 0.0;
}

void Sensor::reset_vertical_angle() {
  Serial.println("sensor reset vertical rotation");
  vertical_angle = 0.0;  // TODO: (scannig_direction & SCANNING_VERTICALLY) ? -MAX_SENSOR_VERTICAL_ROTATION : 0.0;
}

void Sensor::reset_height() {
  Serial.println("sensor reset height");
  height = 0;
}

void Sensor::reset() {
  reset_horizontal_angle();
  reset_vertical_angle();
  reset_height();
}