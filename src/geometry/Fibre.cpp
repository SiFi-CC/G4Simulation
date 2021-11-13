#include "Fibre.h"
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

G4LogicalVolume* Fibre::Construct()
{

    auto fibre =
        new G4LogicalVolume(new G4Box("fibreWrapperSolid", fLength / 2, fWidth / 2, fThickness / 2),
                            fCouplingMaterial, "fibreWrapperLogical");

    // wrapping width is hardcoded as 0.014 mm
    // auto fibreWrapping =
    //     new G4LogicalVolume(new G4Box("fibreWrappingSolid", fLength / 2, 1.014 / 2, 1.014 / 2),
    //                         fWrappingMaterial, "fibreWrappingLogical");

    // actual fibre width is hardcoded as 1 mm
    auto actualfibre = new G4LogicalVolume(new G4Box("fibreSolid", fLength / 2, fWidth/ 2, fThickness / 2),
                                           fFibreMaterial, "fibreLogical");


    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), actualfibre, "fibrephysical", fibre, 0, 1,
                      0);

    // fibreWrapping->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));
    actualfibre->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
    fibre->SetVisAttributes(G4VisAttributes::Invisible);

    return fibre;
}
} // namespace SiFi
