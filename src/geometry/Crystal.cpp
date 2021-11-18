#include "Crystal.h"
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

G4LogicalVolume* Crystal::Construct()
{

    auto crystal =
        new G4LogicalVolume(new G4Box("crystalWrapperSolid", fsizeX / 2, fsizeY / 2, fThickness / 2),
                            fCouplingMaterial, "crystalWrapperLogical");

    // wrapping width is hardcoded as 0.014 mm
    // auto crystalWrapping =
    //     new G4LogicalVolume(new G4Box("crystalWrappingSolid", fsizeX / 2, 1.014 / 2, 1.014 / 2),
    //                         fWrappingMaterial, "crystalWrappingLogical");

    // actual crystal width is hardcoded as 1 mm
    auto actualcrystal = new G4LogicalVolume(new G4Box("crystalSolid", fsizeX / 2, fsizeY/ 2, fThickness / 2),
                                           fCrystalMaterial, "crystalLogical");


    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), actualcrystal, "crystalphysical", crystal, 0, 1,
                      0);

    // crystalWrapping->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));
    actualcrystal->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
    crystal->SetVisAttributes(G4VisAttributes::Invisible);

    return crystal;
}
} // namespace SiFi
