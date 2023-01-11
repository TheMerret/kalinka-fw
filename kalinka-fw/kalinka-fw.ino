#include <Math.h>

#include "Scanner.h"
#include "Sensor.h"
#include "Utils.h"

#define SENSOR1_HEIGHT_PIN 1
#define SENSOR1_HORIZONTAL_PIN 2
#define SENSOR1_VERTICAL_PIN 3
#define SENSOR1_SENSOR_PIN 4

#define SENSOR2_HEIGHT_PIN 5
#define SENSOR2_HORIZONTAL_PIN 6
#define SENSOR2_VERTICAL_PIN 7
#define SENSOR2_SENSOR_PIN 8

#define SCANER_SCEN_PIN 7

const bool LOGGING = false;

enum struct ScanningState {
  Start,
  Stop,
  Scanning,
  Pause,
  Resume,
  Pending,  
};

const float MAX_SCENE_ROTATION_STEP = 30.0;  // degree
const float MAX_SCENE_ROTATION = 180.0;
const float SENSOR_HEIGHT_STEP = 1.0;
const float SCENE_ROTATION_STEP = 1.0;
const float SENSOR_HORIZONTAL_ROTATION_STEP = 1.0;
const float SENSOR_VERTICAL_ROTATION_STEP = 1.0;
const float SENSOR_MAX_HEIGHT = 3.0;

enum struct ScanningDirection {
  Horizontally = 1,
  Vertically = 2,
  Both =  3,
};

inline bool operator&(ScanningDirection a, ScanningDirection b)
{
    return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
}


ScanningState state = ScanningState::Pending;
ScanningDirection scannig_direction = ScanningDirection::Horizontally;

void setup() {
  Serial.begin(9600);
  while (!Serial); 
  Serial.write(255);  // TODO: normal handshake
}

Sensor sensor1(SENSOR1_HEIGHT_PIN, SENSOR1_HORIZONTAL_PIN, SENSOR1_VERTICAL_PIN, SENSOR1_SENSOR_PIN);
Sensor sensor2(SENSOR2_HEIGHT_PIN, SENSOR2_HORIZONTAL_PIN, SENSOR2_VERTICAL_PIN, SENSOR2_SENSOR_PIN);
Scanner scanner(SCANER_SCEN_PIN, sensor1, sensor2);

struct SensorData {
  float distance = 0;
};

struct SensorsData {
  SensorData sensor1;
  SensorData sensor2;
};

ScanningState parse_command(byte command, ScanningState prev_state) {
  ScanningState new_state;
  switch (command) {
    case 1:
      if (prev_state == ScanningState::Stop or prev_state == ScanningState::Pending) {
          new_state = ScanningState::Start;
      };
      break;
    case 0:
      if (prev_state == ScanningState::Scanning) {
          new_state = ScanningState::Stop;
      };
      break;
    case 3:
      if (prev_state == ScanningState::Scanning) {
          new_state = ScanningState::Pause;
      };
      break;
    case 4:
      if (prev_state == ScanningState::Pause) {
          new_state = ScanningState::Resume;
      };
      break;
    default:
      Serial.println("error");
  };
  return new_state;
}

struct SensorsData capture_sensors_test() {
  SensorsData sd;
  sd.sensor1.distance = get_distance_to_cube(scanner.getSceneAngle(), sensor1.getHorizontalAngle(), sensor1.getHeight(), sensor1.getVerticalAngle() + 90); 
  sd.sensor2.distance = get_distance_to_cube(scanner.getSceneAngle() + 180.0, sensor2.getHorizontalAngle(), sensor2.getHeight(), sensor2.getVerticalAngle() + 90);
  return sd;
}

struct SensorsData capture_sensors() {
  SensorsData sd;  // TODO: store last distance in the sensor itself
  sd.sensor1.distance = sensor1.captureDistance();
  sd.sensor2.distance = sensor2.captureDistance();
  return sd;
}

void send_scanning_data(SensorsData sensors_data) {
  if (LOGGING) {
    Serial.println(scanner.getSceneAngle());
    Serial.print(sensor1.getHeight());
    Serial.print(",");
    Serial.print(sensor1.getHorizontalAngle());
    Serial.print(",");
    Serial.print(sensor1.getVerticalAngle());
    Serial.print(",");
    Serial.println(sensors_data.sensor1.distance);
    Serial.print(sensor2.getHeight());
    Serial.print(",");
    Serial.print(sensor2.getHorizontalAngle());
    Serial.print(",");
    Serial.print(sensor2.getVerticalAngle());
    Serial.print(",");
    Serial.println(sensors_data.sensor2.distance);
  } else {
    // TODO: move data printing into method (?Scanner's)
    float sceneAngle = scanner.getSceneAngle();
    float height1 = sensor1.getHeight();
    float horizontalAngle1 = sensor1.getHorizontalAngle();
    float verticalAngle1 = sensor1.getVerticalAngle();
    float height2 = sensor1.getHeight();
    float horizontalAngle2 = sensor1.getHorizontalAngle();
    float verticalAngle2 = sensor1.getVerticalAngle();
    Serial.write((uint8_t*)&sceneAngle, 4);
    Serial.write((uint8_t*)&height1, 4);
    Serial.write((uint8_t*)&horizontalAngle1, 4);
    Serial.write((uint8_t*)&verticalAngle1, 4);
    Serial.write((uint8_t*)&sensors_data.sensor1.distance, 4);
    Serial.write((uint8_t*)&height2, 4);
    Serial.write((uint8_t*)&horizontalAngle2, 4);
    Serial.write((uint8_t*)&verticalAngle2, 4);
    Serial.write((uint8_t*)&sensors_data.sensor2.distance, 4);
  }
}

void process_sensors() {
  SensorsData sensors_data = capture_sensors();
  send_scanning_data(sensors_data);
}

ScanningState scan_next_step() {
  if ((scanner.getSceneAngle() + SCENE_ROTATION_STEP == MAX_SCENE_ROTATION) and
  (sensor1.getHeight() + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT)) {
    return ScanningState::Stop;
  };
  if (scannig_direction & ScanningDirection::Vertically) {
    if (sensor1.getVerticalAngle() != MAX_SENSOR_VERTICAL_ROTATION) {
      sensor1.rotateVertically(SENSOR_VERTICAL_ROTATION_STEP);
      sensor2.rotateVertically(SENSOR_VERTICAL_ROTATION_STEP);
      return ScanningState::Scanning;
    } else {
      sensor1.resetVerticalAngle();
      sensor2.resetVerticalAngle();
    }
  }
  if ((int)(scanner.getSceneAngle() + SCENE_ROTATION_STEP) % (int)MAX_SCENE_ROTATION_STEP == 0) {
    if (scannig_direction & ScanningDirection::Horizontally) {
      sensor1.resetHorizontalAngle();
      sensor2.resetHorizontalAngle();
    };
    if (sensor1.getHeight() + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT) {
      sensor1.resetHorizontalAngle();
      sensor2.resetHorizontalAngle();
      scanner.rotateScene(+SCENE_ROTATION_STEP);  // по часовой
    } else {
      scanner.rotateScene(-MAX_SCENE_ROTATION_STEP + SCENE_ROTATION_STEP);
      sensor1.raise(SENSOR_HEIGHT_STEP);
      sensor2.raise(-SENSOR_HEIGHT_STEP);
    }
    return ScanningState::Scanning;  
  };
  scanner.rotateScene(+SCENE_ROTATION_STEP);  // по часовой
  sensor1.rotateHorizontally(-SENSOR_HORIZONTAL_ROTATION_STEP); // в другую от стола
  sensor2.rotateHorizontally(-SENSOR_HORIZONTAL_ROTATION_STEP);
  return ScanningState::Scanning; 
}

void stop_scanning() {
  // Serial.println("stop scanning");
}

void loop() {
  if (Serial.available() > 0) {
    byte command = Serial.parseInt();
    state = parse_command(command, state);
  }
  switch (state) {
    case ScanningState::Start:
      state = ScanningState::Scanning;
      scanner.reset();
      break;
    case ScanningState::Scanning:
      process_sensors();
      state = scan_next_step();
      break;
    case ScanningState::Stop:
      stop_scanning();
      state = ScanningState::Pending;
      break;
    case ScanningState::Pause:
      {};
    case ScanningState::Resume:
      state = ScanningState::Scanning;
      break;
    case ScanningState::Pending:
      {};
  }
}
