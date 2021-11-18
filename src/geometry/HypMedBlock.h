#pragma once

#include "DataStorage.h"
#include "CrystalLayer.h"
#include <G4LogicalVolume.hh>

namespace SiFi
{

class HypMedBlock : public DetectorElement
{
public:
    HypMedBlock(const CrystalLayer& layer0, const CrystalLayer& layer1, const CrystalLayer& layer2)
        : fLayer0(layer0), fLayer1(layer1), fLayer2(layer2){};

    G4LogicalVolume* Construct() override;

    double getThickness() { 
        return fLayer0.getThickness() +
               fLayer1.getThickness() + 
               fLayer2.getThickness(); };
    double getSizeX() { 
        return std::max({fLayer0.getSizeX(),
                        fLayer1.getSizeX(),
                        fLayer2.getSizeX()}); };
    double getSizeY() { 
        return std::max({fLayer0.getSizeY(),
                        fLayer1.getSizeY(),
                        fLayer2.getSizeY()}); };
    void writeMetadata(DataStorage* storage);

private:
    int fNumberOfLayers;
    CrystalLayer fLayer0;
    CrystalLayer fLayer1;
    CrystalLayer fLayer2;
};

} // namespace SiFi
