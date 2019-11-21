#include "DetectorBlock.h"
#include "MaterialManager.h"
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVReplica.hh>
#include <G4VisAttributes.hh>
#include <TParameter.h>

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

void DetectorBlock::writeMetadata(DataStorage* storage) {
    double detZPosition = storage->getMetadataNumber("sourceToMaskDistance") +
                          storage->getMetadataNumber("maskToDetectorDistance");
    storage->writeMetadata("detectorMinX", -fLayer.getSizeX() / 2);
    storage->writeMetadata("detectorMaxX", fLayer.getSizeX() / 2);
    storage->writeMetadata("detectorMinY", -fLayer.getSizeY() / 2);
    storage->writeMetadata("detectorMaxY", fLayer.getSizeY() / 2);
    storage->writeMetadata("detectorMinZ", detZPosition - getThickness() / 2);
    storage->writeMetadata("detectorMaxZ", detZPosition + getThickness() / 2);
    storage->writeMetadata("detectorBinX", fLayer.getNumberOfStrips());
    storage->writeMetadata("detectorBinY", fLayer.getNumberOfStrips());
}

} // namespace SiFi
