#ifndef SENSOR_H
#define SENSOR_H

#define MAX_SENSOR_VERTICAL_ROTATION 45.0

class Sensor {
private:
  int heightPin;
  int horizontalPin;
  int verticalPin;
  int sensorPin;

  float height = 0.0;
  float horizontalAngle = 0.0;
  float verticalAngle = 0.0;

public:
  Sensor(const int htp, const int hlp, const int vp, const int sp)
    : heightPin(htp), horizontalPin(hlp), verticalPin(vp), sensorPin(sp) {}
  
  void reset();
  void resetHeight();
  void resetHorizontalAngle();
  void resetVerticalAngle();
  void rotateVertically(float);
  void rotateHorizontally(float);
  void raise(float);
  float captureDistance();

  float getHeight() {
    return height;
  }
  float getHorizontalAngle() {
    return horizontalAngle;
  }
  float getVerticalAngle() {
    return verticalAngle;
  }  
};

#endif