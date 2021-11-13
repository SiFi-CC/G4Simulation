#pragma once

#include "Element.h"
#include "Crystal.h"

#include "CmdLineConfig.hh"

namespace SiFi
{

class CrystalLayer : public DetectorElement
{
public:
    CrystalLayer(int nStripsX, int nStripsY, const Crystal& crystal)
        : fNumberOfStripsX(nStripsX), fNumberOfStripsY(nStripsY), fCrystal(crystal){};

    G4LogicalVolume* Construct() override;

    double getSizeX();
    double getSizeY();
    double getThickness();
    int getNumberOfStripsX();
    int getNumberOfStripsY();

private:
    int fNumberOfStripsX;
    int fNumberOfStripsY;
    Crystal fCrystal;
};

} // namespace SiFi
