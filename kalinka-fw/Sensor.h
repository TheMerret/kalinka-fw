#ifndef SENSOR_H
#define SENSOR_H

#define MAX_SENSOR_VERTICAL_ROTATION 45.0

class Sensor {
  public:
    void reset();
    void resetHeight();
    void resetHorizontalAngle();    
    void resetVerticalAngle();
    void rotateVertically(float);
    void rotateHorizontally(float);
    void raise(float);

  private:
    float height = 0.0;
    float horizontalAngle = 0.0;
    float verticalAngle = 0.0;
  
};

#endif