#pragma once

#include "Element.h"
#include "Fibre.h"

namespace SiFi {

class FibreLayer : public DetectorElement {
  public:
    FibreLayer(int nStrips, const Fibre& fibre)
        : fNumberOfStrips(nStrips), fFibre(fibre){};

    G4LogicalVolume* Construct() override;

    double getSizeX() { return fFibre.getLength(); };
    double getSizeY() { return fNumberOfStrips * fFibre.getWidth(); };
    double getThickness() { return fFibre.getThickness(); };

  private:
    int fNumberOfStrips;
    Fibre fFibre;
};

} // namespace SiFi
