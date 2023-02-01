#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

// TODO: debug macross in whole lib
#define DEBUG_ON 1
#define DEBUG_OFF 0
const byte debugMode = DEBUG_OFF;
#define DBGLN(...) debugMode == DEBUG_ON ? Serial.println(__VA_ARGS__) : NULL
#define DBG(...) debugMode == DEBUG_ON ? Serial.print(__VA_ARGS__) : NULL

struct SensorPacket {
  float height;
  float horizontalAngle;
  float verticalAngle;
  float distance;
  SensorPacket() = default;
  SensorPacket(float h, float ha, float va, float d)
    : height(h), horizontalAngle(ha), verticalAngle(va), distance(d) {}
  byte *toBytes();
};

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
    SensorPacket read();
};

#endif
