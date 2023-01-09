#ifndef SENSOR_H
#define SENSOR_H

#define MAX_SENSOR_VERTICAL_ROTATION 45.0

class Sensor {
  public:
    void reset();
    void reset_height();
    void reset_horizontal_angle();    
    void reset_vertical_angle();
    void rotate_vertically(float);
    void rotate_horizontally(float);
    void raise(float);

  private:
    float height = 0.0;
    float horizontal_angle = 0.0;
    float vertical_angle = 0.0;
  
};

#endif