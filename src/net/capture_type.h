
#ifndef _NET_CAPTURE_TYPE_H
#define _NET_CAPTURE_TYPE_H

namespace mckeys {


enum _capture_type {
  CT_SET,
  CT_GET
};

class CaptureType {
 public:
  static CaptureType SET;
  static CaptureType GET;

  static CaptureType fromString(const std::string &name);

  bool operator==(const CaptureType &other) const;

  enum _capture_type getType() const;
  std::string getName() const;

 protected:
  std::string name;
  enum _capture_type type;

  CaptureType(const std::string &name, const enum _capture_type type);

};

inline std::string CaptureType::getName() const {
    return name;
}

inline enum _capture_type CaptureType::getType() const {
  return type;
}



} // namespace mckeys

#endif
