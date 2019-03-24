#pragma once

#include "FibreLayer.h"
#include <Geant4/G4LogicalVolume.hh>

namespace SiFi {

class DetectorBlock : public DetectorElement {
  public:
    DetectorBlock(int nLayers, const FibreLayer& layer)
        : fLayer(layer), fNumberOfLayers(nLayers){};

    G4LogicalVolume* Construct() override;

  private:
    int fNumberOfLayers;
    FibreLayer fLayer;
};

} // namespace SiFi
