#include <Arduino.h>
#include <Multiservo.h>
#include <AccelStepper.h>

#include "Sensor.h"

Multiservo SERVO_SENSOR_HORIZONTAL;
Multiservo SERVO_SENSOR_VERTICAL;

#define motorInterfaceType 1;
AccelStepper myStepper(motorInterfaceType, heightPinStep, heightPinDir);

const byte enablePin = 8;

void setup() {
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  
  SERVO_SENSOR_VERTICAL.attach(verticalPin);
  SERVO_SENSOR_HORIZONTAL.attach(horizontalPin);

  myStepper.setMaxSpeed(1000);
  myStepper.setAcceleration(50);
  myStepper.setSpeed(200);
}


//+
float Sensor::captureDistance() {
  // TOD0: do only if all task done: scene rotated, scanners tasks done (raised, rotated), buffer not full
  DBGLN("sensor capture distance");
  float volts = analogRead(sensorPin) * 0.0048828125;
  return 13 * pow(volts, -1);
}
//+
void Sensor::rotateVertically(float degree) {
  // TODO: do only if not rotatating right now  
  SERVO_SENSOR_VERTICAL.write(verticalAngle + degree);
  DBG("sensor 1 rotate vertically on ");
  DBGLN(degree);
  verticalAngle += degree;
}
//+
void Sensor::rotateHorizontally(float degree) {
  // TODO: do only if not rotatating right now
  SERVO_SENSOR_HORIZONTAL.write(horizontalAngle + degree);
  DBG("sensor 1 rotate horizontally on ");
  DBGLN(degree);
  horizontalAngle += degree;
}
//+
void Sensor::raise(float h) {
  // TODO: do only if not raising right now
  myStepper.moveTo(h);
  myStepper.run();
  DBG("sensor 1 raise on ");
  DBGLN(height);
  height += h;
}
//+
void Sensor::resetHorizontalAngle() {
  // TODO: do only if not rotatating right now
  SERVO_SENSOR_HORIZONTAL.write(90);
  DBGLN("reset reset horizontal rotation");
  horizontalAngle = 0.0;
}
//+
void Sensor::resetVerticalAngle() {
  // TODO: do only if not rotatating right now
  SERVO_SENSOR_VERTICAL.write(90);
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
