#pragma once

#include "Defaults.h"
#include "MaterialManager.h"
#include "Utils.h"
#include "geometry/Element.h"

#include <Geant4/G4Box.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4NistManager.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4Tubs.hh>
#include <Geant4/G4VUserDetectorConstruction.hh>
#include <Geant4/G4VisAttributes.hh>

namespace SiFi {

class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
    DetectorConstruction(
        DetectorElement* mask, DetectorElement* absorber, double distance)
        : fMask(mask), fAbsober(absorber), fDistance(distance){};

    G4VPhysicalVolume* Construct() override {
        log->debug("Construct()");

        auto world = new G4LogicalVolume(
            new G4Box("world", 1.0 * m, 3.0 * m, 3.0 * m),
            MaterialManager::get()->GetMaterial("G4_AIR"),
            "world");

        auto detPos = 50 * cm;

        new G4PVPlacement(
            nullptr,
            G4ThreeVector(0, 0, detPos),
            fAbsober->Construct(),
            "detector",
            world,
            false,
            0);

        new G4PVPlacement(
            nullptr,
            G4ThreeVector(0, 0, detPos - fDistance),
            fMask->Construct(),
            "mask",
            world,
            false,
            0);

        world->SetVisAttributes(G4VisAttributes::Invisible);
        return new G4PVPlacement(
            nullptr, G4ThreeVector(), world, "world", nullptr, false, 0);
    };

    const logger log = createLogger("DetectorConstruction");

  private:
    DetectorElement* fMask;
    DetectorElement* fAbsober;
    double fDistance;
};

} // namespace SiFi
