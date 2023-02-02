#include "Buffer.h"

StaticJsonDocument<BUFFER_DOCUMENT_CAPACITY> Buffer::serialize() {
  return doc;
}

void Buffer::write(SensorPacket sp) {
  if (index == BUFFER_SIZE) {
    // TODO: raise error
    return;
  }
  bool res = array.add(sp.serialize());
  index += 1;
}

void Buffer::clear() {
  doc.clear();
  array = doc.to<JsonArray>();
  index = 0;  // TODO: may be a better way
}
