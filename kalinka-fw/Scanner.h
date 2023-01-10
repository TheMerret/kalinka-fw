#ifndef SCANNER_H
#define SCANNER_H

#include "Sensor.h"

class Scanner {
  public:
    void reset();
    void rotateScene(float);

  private:
    void resetSensors();
    void resetSceneOrigin();

    float sceneAngle = 0.0;
    Sensor sensor1;
    Sensor sensor2;
};

#endif