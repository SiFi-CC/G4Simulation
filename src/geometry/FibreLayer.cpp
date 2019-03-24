#include "FibreLayer.h"

#include "MaterialManager.h"
#include <Geant4/G4Box.hh>
#include <Geant4/G4PVReplica.hh>
#include <Geant4/G4VisAttributes.hh>

namespace SiFi {

G4LogicalVolume* FibreLayer::Construct() {
    auto layer = new G4LogicalVolume(
        new G4Box("fibreLayerSolid", GetSizeX() / 2, GetSizeY() / 2, GetThickness() / 2),
        MaterialManager::get()->Vacuum(),
        "fibreLayerLogical");

    auto fibre = fFibre.Construct();
    layer->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVReplica(
        "fibreLayerRepFibre", fibre, layer, kYAxis, fNumberOfStrips, fFibre.GetWidth());

    return layer;
}

} // namespace SiFi
