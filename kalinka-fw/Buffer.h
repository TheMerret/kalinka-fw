#ifndef BUFFER_H
#define BUFFER_H

#include "Sensor.h"

#define BUFFER_SIZE 10

class Buffer {
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

#endif