#pragma once

#include "DataStorage.h"
#include "FibreLayer.h"
#include <G4LogicalVolume.hh>

namespace SiFi {

class DetectorBlock : public DetectorElement {
  public:
    DetectorBlock(int nLayers, double position, const FibreLayer& layer)
        : fLayer(layer), fPosition(position), fNumberOfLayers(nLayers){};

    G4LogicalVolume* Construct() override;

    double getThickness() { return fLayer.getThickness() * fNumberOfLayers; };
    double getSizeX() { return fLayer.getSizeX(); };
    double getSizeY() { return fLayer.getSizeY(); };
    double getPosition() { return fPosition; };
    void writeMetadata(DataStorage* storage);

  private:
    int fNumberOfLayers;
    FibreLayer fLayer;
    double fPosition;
};

} // namespace SiFi
