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
            fLayer.getSizeX() / 2,
            fLayer.getSizeY() / 2,
            fNumberOfLayers * fLayer.getThickness() / 2),
        MaterialManager::get()->Vacuum(),
        "detectorBlockLogical");

    auto layer = fLayer.Construct();
    new G4PVReplica(
        "detectorBlockRepLayers",
        layer,
        block,
        kZAxis,
        fNumberOfLayers,
        fLayer.getThickness());

    block->SetVisAttributes(G4VisAttributes::Invisible);
    return block;
}

} // namespace SiFi
