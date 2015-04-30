
#include <stdexcept>
#include "common.h"
#include "net/capture_type.h"

namespace mckeys {

// protected constructor
CaptureType::CaptureType(const std::string &name, const enum _capture_type type) : name(name), type(type)
{}

// static values
CaptureType CaptureType::SET = CaptureType("SET", CT_SET);
CaptureType CaptureType::GET = CaptureType("GET", CT_GET);

// static methods
CaptureType CaptureType::fromString(const std::string &name) {
    std::string captureName = "";
  for (uint32_t i = 0; i < name.length(); i++) {
    captureName += toupper(name.at(i));
  }
  if (captureName == "SET") {
    return CaptureType::SET;
  } else if (captureName == "GET") {
    return CaptureType::GET;
  } else {
    throw std::range_error("No such capture type with name " + name);
  }
}


bool CaptureType::operator==(const CaptureType&other) const {
  return (this->getType() == other.getType());
}

} // namespace mckeys
