#include "HypMedBlock.h"
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

G4LogicalVolume* HypMedBlock::Construct()
{
    auto block = new G4LogicalVolume(
        new G4Box("hypmedBlockSolid", getSizeX() / 2,
                  getSizeY() / 2, // the length of fibre defines both sizes
                  getThickness() / 2),       // HYPMED TOTALthickness
        MaterialManager::get()->Vacuum(), "hypmedBlockLogical");

    auto layer0 = fLayer0.Construct();
    auto layer1 = fLayer1.Construct();
    auto layer2 = fLayer2.Construct();

    new G4PVPlacement(
        0,
        G4ThreeVector(0, 0, 0.5*(fLayer0.getThickness()-getThickness())),
        layer0, "hypmedBlockRepLayers", block, 0, 0, 0);
    spdlog::debug("layer0 {}", 0.5*(fLayer0.getThickness()-getThickness()));
    new G4PVPlacement(
        0,
        G4ThreeVector(0, 0,
                      0.5 * (fLayer1.getThickness() + 2 * fLayer0.getThickness() - getThickness())),
        layer1, "hypmedBlockRepLayers", block, 0, 1, 0);
    spdlog::debug("layer1 {}",
                  0.5 * (fLayer1.getThickness() + 2 * fLayer0.getThickness() - getThickness()));

    new G4PVPlacement(
        0,
        G4ThreeVector(0, 0, 0.5 * (fLayer2.getThickness() + 
                    2*fLayer1.getThickness() +
                    2 * fLayer0.getThickness() -
                    getThickness())),
        layer2, "hypmedBlockRepLayers", block, 0, 2, 0);
    spdlog::debug("layer2 {}", 0.5 * (fLayer2.getThickness() + 2 * fLayer1.getThickness() +
                                      2 * fLayer0.getThickness() - getThickness()));

    block->SetVisAttributes(G4VisAttributes::Invisible);
    return block;
}

void HypMedBlock::writeMetadata(DataStorage* storage)
{
    double detZPosition = storage->getMetadataNumber("sourceToMaskDistance") +
                          storage->getMetadataNumber("maskToHypMedDistance");

    storage->writeMetadata("layer0MinX", -fLayer0.getSizeX() / 2);
    storage->writeMetadata("layer0MaxX", fLayer0.getSizeX() / 2);
    storage->writeMetadata("layer0MinY", -fLayer0.getSizeY() / 2);
    storage->writeMetadata("layer0MaxY", fLayer0.getSizeY() / 2);
    storage->writeMetadata("layer0MinZ", detZPosition);
    storage->writeMetadata("layer0MaxZ", detZPosition + getThickness());
    storage->writeMetadata("layer0BinX", fLayer0.getNumberOfStripsX());
    storage->writeMetadata("layer0BinY", fLayer0.getNumberOfStripsY());

    storage->writeMetadata("layer1MinX", -fLayer1.getSizeX() / 2);
    storage->writeMetadata("layer1MaxX", fLayer1.getSizeX() / 2);
    storage->writeMetadata("layer1MinY", -fLayer1.getSizeY() / 2);
    storage->writeMetadata("layer1MaxY", fLayer1.getSizeY() / 2);
    storage->writeMetadata("layer1MinZ", detZPosition);
    storage->writeMetadata("layer1MaxZ", detZPosition + getThickness());
    storage->writeMetadata("layer1BinX", fLayer1.getNumberOfStripsX());
    storage->writeMetadata("layer1BinY", fLayer1.getNumberOfStripsY());

    storage->writeMetadata("layer2MinX", -fLayer2.getSizeX() / 2);
    storage->writeMetadata("layer2MaxX", fLayer2.getSizeX() / 2);
    storage->writeMetadata("layer2MinY", -fLayer2.getSizeY() / 2);
    storage->writeMetadata("layer2MaxY", fLayer2.getSizeY() / 2);
    storage->writeMetadata("layer2MinZ", detZPosition);
    storage->writeMetadata("layer2MaxZ", detZPosition + getThickness());
    storage->writeMetadata("layer2BinX", fLayer2.getNumberOfStripsX());
    storage->writeMetadata("layer2BinY", fLayer2.getNumberOfStripsY());
}


} // namespace SiFi
