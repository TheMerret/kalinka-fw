#include <Arduino.h>

#include "Scanner.h"

void Scanner::rotateScene(float degree) {
  Serial.print("rotate scene on ");
  Serial.println(degree);
  sceneAngle += degree;
}


void Scanner::resetSceneOrigin() {
  // TODO: logging flag
  Serial.println("scene reset rotation");
  sceneAngle = 0;
}

void Scanner::resetSensors() {
  sensor1.reset();
  sensor2.reset();
}

void Scanner::reset() {
  resetSensors();
  resetSceneOrigin();
}