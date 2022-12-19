#pragma once

#include "Element.h"
#include "Fibre.h"

#include "CmdLineConfig.hh"

namespace SiFi
{

class FibreLayer_Scatterrer : public DetectorElement
{
public:
    FibreLayer_Scatterrer(int nStrips, const Fibre& fibre) : fNumberOfStrips(nStrips), fFibre(fibre){};

    G4LogicalVolume* Construct() override;

    double getSizeX();
    double getSizeY();
    double getThickness() { return fFibre.getWidth(); };
    int getNumberOfStrips() { return fNumberOfStrips; };

private:
    int fNumberOfStrips;
    Fibre fFibre;
};

} // namespace SiFi
