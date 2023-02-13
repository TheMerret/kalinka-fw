#define SENSOR1_HEIGHT_PIN_STEP   3   //stepper pin step Y
#define SENSOR1_HEIGHT_PIN_DIR    6   //stepper pin dir Y
#define SENSOR1_HORIZONTAL_PIN    20   //servo pin
#define SENSOR1_VERTICAL_PIN      21  //
#define SENSOR1_SENSOR_PIN        30   //

#define SENSOR2_HEIGHT_PIN_STEP   4   //stepper pin step Z
#define SENSOR2_HEIGHT_PIN_DIR    7   //stepper pin dir Z
#define SENSOR2_HORIZONTAL_PIN    22   //servo pin
#define SENSOR2_VERTICAL_PIN      23  //
#define SENSOR2_SENSOR_PIN        31  //

#define SCANER_SCENE_PIN_STEP     2  // stepper pin step X
#define SCANER_SCENE_PIN_DIR      5  //stepper pin dir X

#include "Scanner.h"
#include "Sensor.h"
#include "Utils.h"

Sensor sensor1(SENSOR1_HEIGHT_PIN_STEP, SENSOR1_HEIGHT_PIN_DIR, SENSOR1_HORIZONTAL_PIN, SENSOR1_VERTICAL_PIN, SENSOR1_SENSOR_PIN, 9);
Sensor sensor2(SENSOR2_HEIGHT_PIN_STEP, SENSOR2_HEIGHT_PIN_DIR, SENSOR2_HORIZONTAL_PIN, SENSOR2_VERTICAL_PIN, SENSOR2_SENSOR_PIN, 10);
Scanner scanner(SCANER_SCENE_PIN_STEP, SCANER_SCENE_PIN_DIR, sensor1, sensor2);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  scanner.attach();
  sensor1.attach();
  sensor2.attach();
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
=======
#define SENSOR1_HEIGHT_PIN 1
#define SENSOR1_HORIZONTAL_PIN 2
#define SENSOR1_VERTICAL_PIN 3
#define SENSOR1_SENSOR_PIN 4

#define SENSOR2_HEIGHT_PIN 5
#define SENSOR2_HORIZONTAL_PIN 6
#define SENSOR2_VERTICAL_PIN 7
#define SENSOR2_SENSOR_PIN 8

#define SCANER_SCENE_PIN 7

#define ARDUINOJSON_ENABLE_INFINITY 1
#include <ArduinoJson.h>

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
    scanner.parseCommand(Serial);
  }
  if (scanner.pointsAvailable() == BUFFER_SIZE) {
    serializeJson(scanner.serialize(), Serial);
    Serial.println();
    scanner.clear();
  }
  scanner.next();
}
