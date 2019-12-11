#pragma once

#include "DataStorage.h"
#include "FibreLayer.h"
#include <G4LogicalVolume.hh>

namespace SiFi {

class DetectorBlock : public DetectorElement {
  public:
    DetectorBlock(int nLayers, const FibreLayer& layer)
        : fNumberOfLayers(nLayers), fLayer(layer)  {};

    G4LogicalVolume* Construct() override;

    double getThickness() { return fLayer.getThickness() * fNumberOfLayers; };
    double getSizeX() { return fLayer.getSizeX(); };
    double getSizeY() { return fLayer.getSizeY(); };
    void writeMetadata(DataStorage* storage);

  private:
    int fNumberOfLayers;
    FibreLayer fLayer;
};

} // namespace SiFi
