#pragma once

#include "MuraMask.h"
#include "Utils.h"

#include <Geant4/G4Box.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4NistManager.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4Tubs.hh>
#include <Geant4/G4VUserDetectorConstruction.hh>

namespace SiFi {

class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
    DetectorConstruction() = default;

    G4VPhysicalVolume* Construct() override {
        log->debug("Construct()");

        auto matManager = G4NistManager::Instance();
        auto air = matManager->FindOrBuildMaterial("G4_AIR");
        auto wolfram = matManager->FindOrBuildMaterial("G4_W");

        auto worldBox = new G4Box("World", 1.0 * m, 3.0 * m, 3.0 * m);
        auto worldLog = new G4LogicalVolume(worldBox, air, "World");
        auto worldPhys = new G4PVPlacement(nullptr, G4ThreeVector(), worldLog, "World", nullptr, false, 0);

        auto detectorBox = new G4Box("detector", 10.0 / 2 * cm, 10.0 / 2 * cm, 5.0 / 2 * cm);
        auto detectorLog = new G4LogicalVolume(detectorBox, wolfram, "detector");
        auto detectorPhys =
            new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 50 * cm), detectorLog, "detector", worldLog, false, 0);

        auto mask = new MuraMask(11, 10 * cm, 10 * cm, 3 * cm, wolfram);
        auto maskPhys =
            new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 30 * cm), mask->logicalVolume(), "mask", worldLog, false, 0);

        return worldPhys;
    };

    const logger log = createLogger("DetectorConstruction");
};

} // namespace SiFi
