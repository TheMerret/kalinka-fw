#ifndef SCANNER_H
#define SCANNER_H

#define BUFFER_SIZE 10

#include "Sensor.h"

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

class Buffer {  // TODO: move class to dedicated header
  private:
    SensorPacket buffer[BUFFER_SIZE];
    int index = 0;
  public:
    void write(SensorPacket sp);
    
    unsigned int size() {
      return index;
    }

    unsigned int sizeRaw() {
      return index * sizeof(buffer[0]);
    }

    byte *toBytes();

    void clear();
};

class Scanner {
  private:
    void resetSensors();
    void resetSceneOrigin();

    const int handshake = 255;

    Buffer buffer;

    int scenePin;

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
    const float SENSOR_MAX_HEIGHT = 3.0;
    ScanningDirection scannig_direction = ScanningDirection::Horizontally;

  public:
    Scanner(const int sp, Sensor snr1, Sensor snr2):
      scenePin(sp), sensor1(snr1), sensor2(snr2) {}

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
    
    // TODO: add stop with buffer clear
};

#endif
