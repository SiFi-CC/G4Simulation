#pragma once

#include "Defaults.h"
#include "MaterialManager.h"
#include "Utils.h"
#include "geometry/Element.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4Tubs.hh>
#include <G4VUserDetectorConstruction.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction(DetectorElement* mask, DetectorElement* absorber)
        : fMask(mask), fAbsober(absorber){};

    G4VPhysicalVolume* Construct() override
    {
        log->debug("Construct()");

        auto world = new G4LogicalVolume(new G4Box("world", .5 * m, .5 * m, 1.2 * m),
                                         MaterialManager::get()->GetMaterial("G4_AIR"), "world");

        new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fDetectorPosZ), fAbsober->Construct(),
                          "detector", world, false, 0);

        new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fMaskPosZ), fMask->Construct(), "mask",
                          world, false, 0);

        world->SetVisAttributes(G4VisAttributes::Invisible);
        return new G4PVPlacement(nullptr, G4ThreeVector(), world, "world", nullptr, false, 0);
    };

    void setMaskPos(double z) { fMaskPosZ = z; };
    void setDetectorPos(double z) { fDetectorPosZ = z; };

    const logger log = createLogger("DetectorConstruction");

private:
    DetectorElement* fMask;
    DetectorElement* fAbsober;
    double fMaskPosZ = 30 * cm;
    double fDetectorPosZ = 50 * cm;
};

} // namespace SiFi
