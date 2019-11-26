#pragma once

#include <G4LogicalVolume.hh>

namespace SiFi {

class DetectorElement {
  public:
    virtual G4LogicalVolume* Construct() = 0;
};

} // namespace SiFi
