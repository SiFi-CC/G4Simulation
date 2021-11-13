#include "DetectorBlock.h"
#include "MaterialManager.h"
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4PVReplica.hh>
#include <G4RotationMatrix.hh>
#include <G4VisAttributes.hh>
#include <TParameter.h>

#include "CmdLineConfig.hh"
#include "Utils.h"

namespace SiFi
{

G4LogicalVolume* DetectorBlock::Construct()
{
    double total_thickness = getThickness();
    auto block = new G4LogicalVolume(
        new G4Box("detectorBlockSolid", getSizeX() / 2,
                  getSizeY() / 2, // the length of fibre defines both sizes
                  total_thickness),       // HYPMED TOTALthickness
        MaterialManager::get()->Vacuum(), "detectorBlockLogical");

    auto layer1 = fLayer1.Construct();
    auto layer2 = fLayer2.Construct();
    auto layer3 = fLayer3.Construct();

    G4RotationMatrix* rotMatrix = new G4RotationMatrix();
    rotMatrix->rotateX(0);
    rotMatrix->rotateY(0);
    rotMatrix->rotateZ(90 * deg);
    // for (int i = 0; i < fNumberOfLayers; i++)
    // {
        // if (i % 2 == 0 || CmdLineOption::GetFlagValue("Single_dimension"))
        // {
    new G4PVPlacement(
        rotMatrix,
        G4ThreeVector(0, 0, 0.5*(fLayer1.getThickness()-total_thickness)),
        layer1, "detectorBlockRepLayers", block, 0, 0, 0);
    spdlog::debug("layer1 {}", 0.5*(fLayer1.getThickness()-total_thickness));
    new G4PVPlacement(
        rotMatrix,
        G4ThreeVector(0, 0,
                      0.5 * (fLayer2.getThickness() + 2 * fLayer1.getThickness() - total_thickness)),
        layer2, "detectorBlockRepLayers", block, 0, 1, 0);
    spdlog::debug("layer2 {}",
                  0.5 * (fLayer2.getThickness() + 2 * fLayer1.getThickness() - total_thickness));

    new G4PVPlacement(
        rotMatrix,
        G4ThreeVector(0, 0, 0.5 * (fLayer3.getThickness() + 
                    2*fLayer2.getThickness() +
                    2 * fLayer1.getThickness() -
                    total_thickness)),
        layer3, "detectorBlockRepLayers", block, 0, 2, 0);
    spdlog::debug("layer3 {}", 0.5 * (fLayer3.getThickness() + 2 * fLayer2.getThickness() +
                                      2 * fLayer1.getThickness() - total_thickness));

    // }
    // else
    // {
    //     new G4PVPlacement(
    //         0, G4ThreeVector(0, 0, (i + 0.5 - 0.5 * fNumberOfLayers) * fLayer.getThickness()),
    //         layer, "detectorBlockRepLayers", block, 0, i, 0);
    // }
    // }

    // For aligned:
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

void DetectorBlock::writeMetadata(DataStorage* storage)
{
    double detZPosition = storage->getMetadataNumber("sourceToMaskDistance") +
                          storage->getMetadataNumber("maskToDetectorDistance");
    storage->writeMetadata("detectorMinX", -fLayer3.getSizeX() / 2);
    storage->writeMetadata("detectorMaxX", fLayer3.getSizeX() / 2);
    storage->writeMetadata("detectorMinY", -fLayer3.getSizeY() / 2);
    storage->writeMetadata("detectorMaxY", fLayer3.getSizeY() / 2);
    storage->writeMetadata("detectorMinZ", detZPosition);
    storage->writeMetadata("detectorMaxZ", detZPosition + getThickness());
    storage->writeMetadata("detectorBinX", fLayer3.getNumberOfStripsX());
    if (CmdLineOption::GetFlagValue("Single_dimension"))
    {
        storage->writeMetadata("detectorBinY", 1);
    }
    else
    {
        storage->writeMetadata("detectorBinY", fLayer3.getNumberOfStripsY());
    }
}


} // namespace SiFi
