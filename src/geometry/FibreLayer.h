#pragma once

#include "Element.h"
#include "Fibre.h"

#include "CmdLineConfig.hh"

namespace SiFi
{

class FibreLayer : public DetectorElement
{
public:
    // FibreLayer(int nStrips, const Fibre& fibre) : fNumberOfStrips(nStrips), fFibre(fibre){};
    FibreLayer(int nStripsX, int nStripsY, const Fibre& fibre)
        : fNumberOfStripsX(nStripsX), fNumberOfStripsY(nStripsY), fFibre(fibre){};

    G4LogicalVolume* Construct() override;

    double getSizeX();
    double getSizeY();
    double getThickness() { return fFibre.getThickness(); };
    int getNumberOfStripsX() { return fNumberOfStripsX; };
    int getNumberOfStripsY() { return fNumberOfStripsY; };

private:
    // int fNumberOfStrips;
    int fNumberOfStripsX;
    int fNumberOfStripsY;
    Fibre fFibre;
};

} // namespace SiFi
