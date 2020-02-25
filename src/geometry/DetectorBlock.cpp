#include "DetectorBlock.h"
#include "MaterialManager.h"
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVReplica.hh>
#include <G4VisAttributes.hh>
#include <G4RotationMatrix.hh>
#include <G4PVPlacement.hh>
#include <TParameter.h>

namespace SiFi {

G4LogicalVolume* DetectorBlock::Construct() {
    auto block = new G4LogicalVolume(
        new G4Box(
            "detectorBlockSolid",
            fLayer.getSizeX() / 2,
            fLayer.getSizeX() / 2, //the length of fibre defines both sizes
            fNumberOfLayers * fLayer.getThickness() / 2),
        MaterialManager::get()->Vacuum(),
        "detectorBlockLogical");

    auto layer = fLayer.Construct();

    G4RotationMatrix* rotMatrix = new G4RotationMatrix();
    rotMatrix->rotateX(0);
    rotMatrix->rotateY(0);
    rotMatrix->rotateZ(90*deg);
    for (int i = 0; i < fNumberOfLayers; i++){
        // std::cout<<(i+0.5-0.5*fNumberOfLayers)*fLayer.getThickness()<<std::endl;
        if(i % 2 == 0){
            new G4PVPlacement(rotMatrix,G4ThreeVector(0,0,(i+0.5-0.5*fNumberOfLayers)*fLayer.getThickness()),
                layer,"detectorBlockRepLayers",block,0,i,0);
        } else {
            new G4PVPlacement(0,G4ThreeVector(0,0,(i+0.5-0.5*fNumberOfLayers)*fLayer.getThickness()),
                layer,"detectorBlockRepLayers",block,0,i,0);
        }
    }

    //For aligned:
    // new G4PVReplica(
    //     "detectorBlockRepLayers",
    //     layer,
    //     block,
    //     kZAxis,
    //     fNumberOfLayers,
    //     fLayer.getThickness());

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
