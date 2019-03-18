#pragma once

#include <Geant4/G4Box.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4SystemOfUnits.hh>

namespace SiFi {

class MuraMask {
  public:
    MuraMask(int n, G4double x, G4double y, G4double z, G4Material* mat);

    G4LogicalVolume* logicalVolume() { return fMaskLog; };

  private:
    G4LogicalVolume* fMaskLog;
};

} // namespace SiFi
