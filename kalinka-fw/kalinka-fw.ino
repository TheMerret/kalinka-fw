#define SENSOR1_HEIGHT_PIN 1
#define SENSOR1_HORIZONTAL_PIN 2
#define SENSOR1_VERTICAL_PIN 3
#define SENSOR1_SENSOR_PIN 4

#define SENSOR2_HEIGHT_PIN 5
#define SENSOR2_HORIZONTAL_PIN 6
#define SENSOR2_VERTICAL_PIN 7
#define SENSOR2_SENSOR_PIN 8

#define SCANER_SCENE_PIN 7

#include "Scanner.h"
#include "Sensor.h"
#include "Utils.h"

Sensor sensor1(SENSOR1_HEIGHT_PIN, SENSOR1_HORIZONTAL_PIN, SENSOR1_VERTICAL_PIN, SENSOR1_SENSOR_PIN);
Sensor sensor2(SENSOR2_HEIGHT_PIN, SENSOR2_HORIZONTAL_PIN, SENSOR2_VERTICAL_PIN, SENSOR2_SENSOR_PIN);
Scanner scanner(SCANER_SCENE_PIN, sensor1, sensor2);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.write(scanner.getHandshake());
}


void loop() {
  if (Serial.available() > 0) {
    byte command = Serial.read();
    scanner.parseCommand(command);
  }
  if (scanner.pointsAvailable() > 10) {
    // writeBuffer(scanner.toBytes(), scanner.bytesLen());
  }
  scanner.next();
}
