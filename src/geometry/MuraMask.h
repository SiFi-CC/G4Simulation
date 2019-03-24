#pragma once

#include "Element.h"

#include <Geant4/G4Box.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4SystemOfUnits.hh>

namespace SiFi {

class MuraMask : public DetectorElement {
  public:
    MuraMask(int order, const G4ThreeVector& size, G4Material* material)
        : fMaskOrder(order), fSize(size), fMaterial(material) {}

    G4LogicalVolume* Construct() override;

    double getThickness() { return fSize.z(); }

  private:
    int fMaskOrder;
    G4ThreeVector fSize;
    G4Material* fMaterial;
};

} // namespace SiFi
