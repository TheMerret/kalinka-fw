#ifndef SCANNER_H
#define SCANNER_H

#include "Sensor.h"

class Scanner {
  public:
    void reset();
    void rotate_scene(float);

  private:
    void reset_sensors();
    void reset_scene_origin();

    float scene_angle = 0.0;
    Sensor sensor1;
    Sensor sensor2;
};

#endif