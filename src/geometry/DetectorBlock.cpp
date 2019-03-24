#include "DetectorBlock.h"
#include "MaterialManager.h"
#include <Geant4/G4Box.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4PVReplica.hh>
#include <Geant4/G4VisAttributes.hh>

namespace SiFi {

G4LogicalVolume* DetectorBlock::Construct() {
    auto block = new G4LogicalVolume(
        new G4Box(
            "detectorBlockSolid",
            fLayer.GetSizeX() / 2,
            fLayer.GetSizeY() / 2,
            fNumberOfLayers * fLayer.GetThickness() / 2),
        MaterialManager::get()->Vacuum(),
        "detectorBlockLogical");

    auto layer = fLayer.Construct();
    new G4PVReplica(
        "detectorBlockRepLayers",
        layer,
        block,
        kZAxis,
        fNumberOfLayers,
        fLayer.GetThickness());

    block->SetVisAttributes(G4VisAttributes::Invisible);
    return block;
}

} // namespace SiFi
