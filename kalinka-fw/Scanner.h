#ifndef SCANNER_H
#define SCANNER_H

#include "Sensor.h"

class Scanner {
private:
  void resetSensors();
  void resetSceneOrigin();

  int scenePin;

  float sceneAngle = 0.0;
  Sensor sensor1;
  Sensor sensor2;

public:
  Scanner(const int sp, Sensor snr1, Sensor snr2):
   scenePin(sp), sensor1(snr1), sensor2(snr2) {}
  void reset();
  void rotateScene(float);
};

#endif