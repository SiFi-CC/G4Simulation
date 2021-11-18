#pragma once

#include "Element.h"
#include <G4LogicalVolume.hh>
#include <G4Material.hh>

namespace SiFi
{

struct CrystalSpecs
{
    double sizeX;
    double sizeY;
    double thickness; // z axis(thickness of single layer)

    G4Material* crystalMaterial;
    G4Material* wrappingMaterial;
    G4Material* couplingMaterial;
};

class Crystal : public DetectorElement
{
public:
    explicit Crystal(const CrystalSpecs& specs)
        : fsizeX(specs.sizeX), fsizeY(specs.sizeY),
          fThickness(specs.thickness),
          fCrystalMaterial(specs.crystalMaterial), fWrappingMaterial(specs.wrappingMaterial),
          fCouplingMaterial(specs.couplingMaterial){};
    G4LogicalVolume* Construct() override;

    double getSizeX() { return fsizeX; };
    double getSizeY() { return fsizeY; };
    double getThickness() { return fThickness; };

private:
    double fsizeX;
    double fsizeY;
    double fThickness;

    G4Material* fCrystalMaterial;
    G4Material* fWrappingMaterial;
    G4Material* fCouplingMaterial;
};

} // namespace SiFi
