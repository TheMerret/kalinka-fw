#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

#include <Servo.h>
#include <AccelStepper.h>
#include <ArduinoJson.h>

#define motorInterfaceType 1
#define enablePin 8

#define SENSOR_DOCUMENT_CAPACITY 48

// TODO: debug macross in whole lib
#define DEBUG_ON 1
#define DEBUG_OFF 0
const byte debugMode = DEBUG_OFF;
#define DBGLN(...) debugMode == DEBUG_ON ? Serial.println(__VA_ARGS__) : NULL
#define DBG(...) debugMode == DEBUG_ON ? Serial.print(__VA_ARGS__) : NULL

struct SensorPacket {
  float sceneAngle;
  float height;
  float horizontalAngle;
  float verticalAngle;
  float distance;
  StaticJsonDocument<SENSOR_DOCUMENT_CAPACITY> doc;
  SensorPacket() = default;
  SensorPacket(float h, float ha, float va, float d)
    : height(h), horizontalAngle(ha), verticalAngle(va), distance(d) {}
  StaticJsonDocument<SENSOR_DOCUMENT_CAPACITY> serialize();
};

class Sensor {
  private:
    int buttonPin;
    int heightPinStep;
    int heightPinDir;
    int horizontalPin;
    int verticalPin;
    int sensorPin;

    float height = 0.0;
    float horizontalAngle = 0.0;
    float verticalAngle = 0.0;

    Servo servo_sensor_horizontal;
    Servo servo_sensor_vertical;

    AccelStepper heightStepper;

  public:
    Sensor::Sensor(const int htps, const int htpd, const int hlp, const int vp, const int sp, const int bp)
    : heightPinStep(htps),
    heightPinDir(htpd),
    horizontalPin(hlp),
    verticalPin(vp),
    sensorPin(sp),
    heightStepper(motorInterfaceType, heightPinStep, heightPinDir),
    buttonPin(bp) {}
    void attach();     

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
