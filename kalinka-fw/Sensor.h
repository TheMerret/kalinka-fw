#ifndef SENSOR_H
#define SENSOR_H

#define MAX_SENSOR_VERTICAL_ROTATION 45.0

class Sensor {
private:
  int heightPin;
  int horizontalPin;
  int verticalPin;

  float height = 0.0;
  float horizontalAngle = 0.0;
  float verticalAngle = 0.0;

public:
  Sensor(const int htp, const int hlp, const int vp)
    : heightPin(htp), horizontalPin(hlp), verticalPin(vp) {}
  void reset();
  void resetHeight();
  void resetHorizontalAngle();
  void resetVerticalAngle();
  void rotateVertically(float);
  void rotateHorizontally(float);
  void raise(float);
  void captureDistance();
};

#endif