#pragma once

#include "Element.h"
#include <G4LogicalVolume.hh>
#include <G4Material.hh>

namespace SiFi
{

struct FibreSpecs
{
    double length;
    double width;
    // double thickness; // z axis(thickness of single layer)

    G4Material* fibreMaterial;
    G4Material* wrappingMaterial;
    G4Material* couplingMaterial;
};

class Fibre : public DetectorElement
{
public:
    explicit Fibre(const FibreSpecs& specs)
        : fLength(specs.length), fWidth(specs.width),
          // fThickness(specs.thickness),
          fFibreMaterial(specs.fibreMaterial), fWrappingMaterial(specs.wrappingMaterial),
          fCouplingMaterial(specs.couplingMaterial){};
    G4LogicalVolume* Construct() override;

    double getLength() { return fLength; };
    double getWidth() { return fWidth; };
    // double getThickness() { return fThickness; };

private:
    double fLength;
    double fWidth;
    // double fThickness;

    G4Material* fFibreMaterial;
    G4Material* fWrappingMaterial;
    G4Material* fCouplingMaterial;
};

} // namespace SiFi
