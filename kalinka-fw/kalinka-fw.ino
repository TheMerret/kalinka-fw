#define SENSOR1_HEIGHT_PIN_STEP 3
#define SENSOR1_HEIGHT_PIN_DIR 6
#define SENSOR1_HORIZONTAL_PIN 2
#define SENSOR1_VERTICAL_PIN 17
#define SENSOR1_SENSOR_PIN 5

#define SENSOR2_HEIGHT_PIN_STEP 6
#define SENSOR2_HEIGHT_PIN_DIR 7
#define SENSOR2_HORIZONTAL_PIN 8
#define SENSOR2_VERTICAL_PIN 14
#define SENSOR2_SENSOR_PIN 15

#define SCANER_SCENE_PIN 11

#include "Scanner.h"
#include "Sensor.h"
#include "Utils.h"

Sensor sensor1(SENSOR1_HEIGHT_PIN_STEP, SENSOR1_HEIGHT_PIN_DIR, SENSOR1_HORIZONTAL_PIN, SENSOR1_VERTICAL_PIN, SENSOR1_SENSOR_PIN, 9);
Sensor sensor2(SENSOR2_HEIGHT_PIN_STEP, SENSOR2_HEIGHT_PIN_DIR, SENSOR2_HORIZONTAL_PIN, SENSOR2_VERTICAL_PIN, SENSOR2_SENSOR_PIN, 10);
Scanner scanner(SCANER_SCENE_PIN, sensor1, sensor2);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("heh");
  Serial.write(scanner.getHandshake());
}


void loop() {
  if (Serial.available() > 0) {
    byte command = Serial.read();
    scanner.parseCommand(command);
  }
  if (scanner.pointsAvailable() == 10) {
    writeBuffer(scanner.toBytes(), scanner.bytesLen());
    scanner.clear();
  }
  scanner.next();
}
