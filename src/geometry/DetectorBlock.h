#pragma once

#include "DataStorage.h"
#include "CrystalLayer.h"
#include <G4LogicalVolume.hh>

namespace SiFi
{

class DetectorBlock : public DetectorElement
{
public:
    // DetectorBlock(int nLayers, const CrystalLayer& layer) : fNumberOfLayers(nLayers), fLayer(layer){};
    DetectorBlock(const CrystalLayer& layer1, const CrystalLayer& layer2, const CrystalLayer& layer3)
        : fLayer1(layer1), fLayer2(layer2), fLayer3(layer3){};

    G4LogicalVolume* Construct() override;

    double getThickness() { 
        return fLayer1.getThickness() +
               fLayer2.getThickness() + 
               fLayer3.getThickness(); };
    double getSizeX() { 
        return std::max({fLayer1.getSizeX(),
                        fLayer2.getSizeX(),
                        fLayer3.getSizeX()}); };
    double getSizeY() { 
        return std::max({fLayer1.getSizeY(),
                        fLayer2.getSizeY(),
                        fLayer3.getSizeY()}); };
    void writeMetadata(DataStorage* storage);

private:
    int fNumberOfLayers;
    // CrystalLayer fLayer;
    CrystalLayer fLayer1;
    CrystalLayer fLayer2;
    CrystalLayer fLayer3;
};

} // namespace SiFi
