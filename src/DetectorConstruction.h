#pragma once

#include <Geant4/G4VUserDetectorConstruction.hh>

namespace SiFi {

class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
    DetectorConstruction() = default;

    G4VPhysicalVolume* Construct() override { return nullptr; };
};

} // namespace SiFi
