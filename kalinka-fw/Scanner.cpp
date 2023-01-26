#include <Arduino.h>

#include "Scanner.h"

Scanner::Scanner(const int sp, Sensor snr1, Sensor snr2):
  scenePin(sp), sensor1(snr1), sensor2(snr2) {
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);

  //  stepper.setMaxSpeed(1000);
  //  stepper.setAcceleration(50);
  //  stepper.setSpeed(200);

  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(1500);
  stepper.setSpeed(5000);
}

void Scanner::rotateScene(float degree) {
  // TODO: do only if not rotatating right now
  stepper.moveTo(degree);
  stepper.run();
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

void Scanner::parseCommand(byte command) {
  switch (command) {
    case '1':
      if (state == ScanningState::Stop or state == ScanningState::Pending) {
        state = ScanningState::Start;
      };
      break;
    case '0':
      if (state == ScanningState::Scanning) {
        state = ScanningState::Stop;
      };
      break;
    case '3':
      if (state == ScanningState::Scanning) {
        state = ScanningState::Pause;
      };
      break;
    case '4':
      if (state == ScanningState::Pause) {
        state = ScanningState::Resume;
      };
      break;
    default:
      Serial.println("error");
  };
}

unsigned int Scanner::pointsAvailable() {
  return buffer.size();
}

void Scanner::move() {
  if ((sceneAngle + SCENE_ROTATION_STEP == MAX_SCENE_ROTATION) and
      (sensor1.getHeight() + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT)) {
    state = ScanningState::Stop;
  };
  if (scannig_direction & ScanningDirection::Vertically) {
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
    if (scannig_direction & ScanningDirection::Horizontally) {
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
  rotateScene(+SCENE_ROTATION_STEP);  // по часовой
  sensor1.rotateHorizontally(-SENSOR_HORIZONTAL_ROTATION_STEP); // в другую от стола
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

byte *Scanner::toBytes() {
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
