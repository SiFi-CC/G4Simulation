#include "Fibre.h"
#include <Geant4/G4Box.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4ThreeVector.hh>
#include <Geant4/G4VisAttributes.hh>

namespace SiFi {

G4LogicalVolume* Fibre::Construct() {

    auto fibre = new G4LogicalVolume(
        new G4Box("fibreWrapperSolid", fLength / 2, fWidth / 2, fThickness / 2),
        fFibreMaterial,
        "fibreWrapperLogical");

    fibre->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

    return fibre;
}
} // namespace SiFi
