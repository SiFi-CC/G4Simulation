#pragma once

#include "DataStorage.h"
#include "Element.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4SystemOfUnits.hh>

namespace SiFi {

class MuraMask : public DetectorElement {
  public:
    MuraMask(int order, const G4ThreeVector& size, G4Material* material)
        : fMaskOrder(order), fSize(size), fMaterial(material) {}

    G4LogicalVolume* Construct() override;

    double getThickness() { return fSize.z(); }
    bool isMaskedAt(int x, int y);
    int isQuaResidue(int q, int p);

    void writeMetadata(DataStorage* storage);

  private:
    int fMaskOrder;
    G4ThreeVector fSize;
    G4Material* fMaterial;
};

} // namespace SiFi
