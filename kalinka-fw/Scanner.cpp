#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "Scanner.h"

void Scanner::rotateScene(float degree) {
  // TODO: do only if not rotatating right now
  DBG("rotate scene on ");
  DBGLN(degree);
  sceneAngle += degree;
}


void Scanner::resetSceneOrigin() {
  // TODO: do only if not rotatating right now
  DBGLN("scene reset rotation");
  sceneAngle = 0.0;
}

void Scanner::resetSensors() {
  sensor1.reset();
  sensor2.reset();
}

void Scanner::reset() {
  resetSensors();
  resetSceneOrigin();
}

void Scanner::parseCommand(HardwareSerial& input) {  // TODO: move to dedicated class
                                                     // TODO: use abstract Stream within Serial
  DeserializationError error = deserializeJson(json_doc, Serial);
  if (error) {
    return;
  }
  const char* cmd = json_doc["cmd"];
  int argc = json_doc["argc"];
  const char* argv[argc];
  for (int i = 0; i < argc; i++) {
    argv[i] = json_doc["argv"][i];
  }
  this->mapCommand(cmd, argc, argv);
}

void Scanner::mapCommand(char* cmd, int argc, char* argv[]) {
  // TODO: callbacks for each command
  if (strcmp(cmd, "start") == 0) {
    if (state == ScanningState::Stop or state == ScanningState::Pending) {
      state = ScanningState::Start;
      DBG("set state to ");
      DBGLN(cmd);
    };
  } else if (strcmp(cmd, "stop") == 0) {
    if (state == ScanningState::Scanning) {
      state = ScanningState::Stop;
      DBG("set state to ");
      DBGLN(cmd);
    };
  } else if (strcmp(cmd, "pause") == 0) {
    if (state == ScanningState::Scanning) {
      state = ScanningState::Pause;
      DBG("set state to ");
      DBGLN(cmd);
    };
  } else if (strcmp(cmd, "resume") == 0) {
    if (state == ScanningState::Pause) {
      state = ScanningState::Resume;
      DBG("set state to ");
      DBGLN(cmd);
    };
  } else if (strcmp(cmd, "set") == 0) {
    char* field = argv[0];
    if (strcmp(field, "SCANNIG_DIRECTION") == 0) {
      char* value = argv[1];
      if (strcmp(value, "horizontally") == 0) {
        SCANNING_DIRECTION = ScanningDirection::Horizontally;
      } else if (strcmp(value, "vertically") == 0) {
        SCANNING_DIRECTION = ScanningDirection::Vertically;
      } else if (strcmp(value, "both") == 0) {
        SCANNING_DIRECTION = ScanningDirection::Both;
      } else {
        return;  // TODO: go to default error
      }
      DBG("set ");
      DBG(field);
      DBG(" to ");
      DBGLN(value);
    } else {
      float value = atof(argv[1]);
      // TODO: use dict or smth
      if (strcmp(field, "MAX_SCENE_ROTATION_STEP") == 0) {
        MAX_SCENE_ROTATION_STEP = value;
      } else if (strcmp(field, "MAX_SCENE_ROTATION") == 0) {
        MAX_SCENE_ROTATION = value;
      } else if (strcmp(field, "SENSOR_HEIGHT_STEP") == 0) {
        SENSOR_HEIGHT_STEP = value;
      } else if (strcmp(field, "SCENE_ROTATION_STEP") == 0) {
        SCENE_ROTATION_STEP = value;
      } else if (strcmp(field, "SENSOR_HORIZONTAL_ROTATION_STEP") == 0) {
        SENSOR_HORIZONTAL_ROTATION_STEP = value;
      } else if (strcmp(field, "SENSOR_VERTICAL_ROTATION_STEP") == 0) {
        SENSOR_VERTICAL_ROTATION_STEP = value;
      } else if (strcmp(field, "MAX_SENSOR_VERTICAL_ROTATION") == 0) {
        MAX_SENSOR_VERTICAL_ROTATION = value;
      } else if (strcmp(field, "SENSOR_MAX_HEIGHT") == 0) {
        SENSOR_MAX_HEIGHT = value;
      } else {
        return;  // TODO: go to default error
      }
      DBG("set ");
      DBG(field);
      DBG(" to ");
      DBGLN(value);
    }
  } else {
    Serial.println("error");  // TODO: better error output
  };
}

unsigned int Scanner::pointsAvailable() {
  return buffer.size();
}

void Scanner::move() {
  if ((sceneAngle + SCENE_ROTATION_STEP == MAX_SCENE_ROTATION) and (sensor1.getHeight() + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT)) {
    state = ScanningState::Stop;
  };
  if (SCANNING_DIRECTION & ScanningDirection::Vertically) {
    if (sensor1.getVerticalAngle() != MAX_SENSOR_VERTICAL_ROTATION) {
      sensor1.rotateVertically(SENSOR_VERTICAL_ROTATION_STEP);
      sensor2.rotateVertically(SENSOR_VERTICAL_ROTATION_STEP);
      state = ScanningState::Scanning;
    } else {
      sensor1.resetVerticalAngle();
      sensor2.resetVerticalAngle();
    }
  }
  if ((int)(sceneAngle + SCENE_ROTATION_STEP) % (int)MAX_SCENE_ROTATION_STEP == 0) {
    if (SCANNING_DIRECTION & ScanningDirection::Horizontally) {
      sensor1.resetHorizontalAngle();
      sensor2.resetHorizontalAngle();
    };
    if (sensor1.getHeight() + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT) {
      sensor1.resetHorizontalAngle();
      sensor2.resetHorizontalAngle();
      rotateScene(+SCENE_ROTATION_STEP);  // по часовой
    } else {
      rotateScene(-MAX_SCENE_ROTATION_STEP + SCENE_ROTATION_STEP);
      sensor1.raise(SENSOR_HEIGHT_STEP);
      sensor2.raise(-SENSOR_HEIGHT_STEP);
    }
    state = ScanningState::Scanning;
  };
  rotateScene(+SCENE_ROTATION_STEP);                             // по часовой
  sensor1.rotateHorizontally(-SENSOR_HORIZONTAL_ROTATION_STEP);  // в другую от стола
  sensor2.rotateHorizontally(-SENSOR_HORIZONTAL_ROTATION_STEP);
  state = ScanningState::Scanning;
}

void Scanner::next() {
  switch (state) {
    case ScanningState::Start:
      state = ScanningState::Scanning;
      reset();
      break;
    case ScanningState::Scanning:
      move();
      readSensors();
      break;
    case ScanningState::Stop:
      stop();
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

void Scanner::readSensors() {
  // TOD0: do only if all task done: scene rotated, scanners tasks done (raised, rotated), buffer not full
  buffer.write(sensor1.read());
  buffer.write(sensor2.read());
}

byte* Scanner::toBytes() {
  byte raw[sizeof(sceneAngle) + buffer.sizeRaw()];
  memcpy(raw, &sceneAngle, sizeof(sceneAngle));
  memcpy(raw + sizeof(sceneAngle), buffer.toBytes(), buffer.sizeRaw());
  return raw;
}

unsigned int Scanner::bytesLen() {
  return sizeof(sceneAngle) + buffer.sizeRaw();
}

void Scanner::clear() {
  buffer.clear();
}

void Scanner::stop() {
  this->clear();
}