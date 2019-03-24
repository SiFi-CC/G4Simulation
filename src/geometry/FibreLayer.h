#pragma once

#include "Element.h"
#include "Fibre.h"

namespace SiFi {

class FibreLayer : public DetectorElement {
  public:
    FibreLayer(int nStrips, const Fibre& fibre)
        : fNumberOfStrips(nStrips), fFibre(fibre){};

    G4LogicalVolume* Construct() override;

    double GetSizeX() { return fFibre.GetLength(); };
    double GetSizeY() { return fNumberOfStrips * fFibre.GetWidth(); };
    double GetThickness() { return fFibre.GetThickness(); };

  private:
    int fNumberOfStrips;
    Fibre fFibre;
};

} // namespace SiFi
