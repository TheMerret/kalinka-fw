#ifndef BUFFER_H
#define BUFFER_H

#include <ArduinoJson.h>

#include "Sensor.h"

#define BUFFER_SIZE 10
#define BUFFER_DOCUMENT_CAPACITY 512

class Buffer {
  private:
    StaticJsonDocument<BUFFER_DOCUMENT_CAPACITY> doc;
    JsonArray array = doc.to<JsonArray>();
    int index = 0;
  public:
    void write(SensorPacket sp);
    
    unsigned int size() {
      return index;
    }

    unsigned int sizeRaw() {
      return BUFFER_DOCUMENT_CAPACITY;
    }

    StaticJsonDocument<BUFFER_DOCUMENT_CAPACITY> serialize();

    void clear();
};

#endif
