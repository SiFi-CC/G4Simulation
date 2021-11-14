#include "FibreLayer.h"

#include "MaterialManager.h"
#include <G4Box.hh>
#include <G4PVReplica.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

G4LogicalVolume* FibreLayer::Construct()
{
    auto layer = new G4LogicalVolume(
        new G4Box("fibreLayerSolid", getSizeX() / 2, getSizeY() / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "fibreLayerLogical");

    auto fibre = fFibre.Construct();
    layer->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVReplica("fibreLayerRepFibre", fibre, layer, kYAxis, fNumberOfStrips, fFibre.getWidth());

    return layer;
}

} // namespace SiFi
