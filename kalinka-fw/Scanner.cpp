#include <Arduino.h>

#include "Scanner.h"

void Scanner::rotate_scene(float degree) {
  Serial.print("rotate scene on ");
  Serial.println(degree);
  scene_angle += degree;
}


void Scanner::reset_scene_origin() {
  // TODO: logging flag
  Serial.println("scene reset rotation");
  scene_angle = 0;
}

void Scanner::reset_sensors() {
  sensor1.reset();
  sensor2.reset();
}

void Scanner::reset() {
  reset_sensors();
  reset_scene_origin();
}