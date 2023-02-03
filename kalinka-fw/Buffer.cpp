#include "Buffer.h"

byte *Buffer::toBytes() {
  byte raw[this->sizeRaw()];
  for (int i=0;i<this->size();i++) {
    memcpy(raw, buffer[i].toBytes(), sizeof(SensorPacket));
  };
  return raw;
}

void Buffer::write(SensorPacket sp) {
  if (index == BUFFER_SIZE) {
    // TODO: raise error
    return;
  }
  buffer[index] = sp;
  index += 1;
}

void Buffer::clear() {
  index = 0;  // TODO: may be a better way
}
