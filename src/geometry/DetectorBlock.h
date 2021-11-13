#pragma once

#include "DataStorage.h"
#include "FibreLayer.h"
#include <G4LogicalVolume.hh>

namespace SiFi
{

class DetectorBlock : public DetectorElement
{
public:
    // DetectorBlock(int nLayers, const FibreLayer& layer) : fNumberOfLayers(nLayers), fLayer(layer){};
    DetectorBlock(const FibreLayer& layer1, const FibreLayer& layer2, const FibreLayer& layer3)
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
    // FibreLayer fLayer;
    FibreLayer fLayer1;
    FibreLayer fLayer2;
    FibreLayer fLayer3;
};

} // namespace SiFi
