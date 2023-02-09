#ifndef SCANNER_H
#define SCANNER_H

#include <AccelStepper.h>

#include "Sensor.h"
#include "Buffer.h"

enum struct ScanningState {
  Start,
  Stop,
  Scanning,
  Pause,
  Resume,
  Pending,
};

enum struct ScanningDirection {
  Horizontally = 1,
  Vertically = 2,
  Both =  3,
};

inline bool operator&(ScanningDirection a, ScanningDirection b)
{
  return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
}

class Scanner {
  private:
    void resetSensors();
    void resetSceneOrigin();

    const int handshake = 255;

    Buffer buffer;

    int scenePinStep;
    int scenePinDir;
    AccelStepper stepper;

    float sceneAngle = 0.0;
    Sensor sensor1;
    Sensor sensor2;

    ScanningState state = ScanningState::Pending;

    // TODO: settable settings
    const float MAX_SCENE_ROTATION_STEP = 30.0;  // degree
    const float MAX_SCENE_ROTATION = 180.0;
    const float SENSOR_HEIGHT_STEP = 1.0;
    const float SCENE_ROTATION_STEP = 1.0;
    const float SENSOR_HORIZONTAL_ROTATION_STEP = 1.0;
    const float SENSOR_VERTICAL_ROTATION_STEP = 1.0;
    const float MAX_SENSOR_VERTICAL_ROTATION = 45.0;
    const float SENSOR_MAX_HEIGHT = 10.0;
    ScanningDirection scannig_direction = ScanningDirection::Horizontally;

  public:
    Scanner::Scanner(const int sps, const int spd, Sensor snr1, Sensor snr2):
    scenePinStep(sps), scenePinDir(spd), sensor1(snr1), sensor2(snr2), stepper(motorInterfaceType, scenePinStep, scenePinDir) {}

    void attach();

    void reset();
    void rotateScene(float);

    float getSceneAngle() {
      return sceneAngle;
    }

    const int getHandshake() {
      return handshake;
    }

    void parseCommand(byte);

    unsigned int pointsAvailable();

    void move();

    void next();

    void readSensors();

    unsigned int bytesLen();

    byte *toBytes();

    void clear();
    
    void stop();
};

#endif
