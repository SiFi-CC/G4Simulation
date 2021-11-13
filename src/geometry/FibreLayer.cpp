#include "FibreLayer.h"

#include "MaterialManager.h"
#include "Utils.h"
#include <G4Box.hh>
#include <G4PVReplica.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

G4LogicalVolume* FibreLayer::Construct()
{
    //changed X and Y - I do not know why it works
    auto row = new G4LogicalVolume(
        new G4Box("fibreRowSolid", getSizeY() / 2, getThickness() / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "fibreRowLogical");

    auto layer = new G4LogicalVolume(
        new G4Box("fibreLayerSolid", getSizeY() / 2, getSizeX() / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "fibreLayerLogical");

    auto fibre = fFibre.Construct();
    layer->SetVisAttributes(G4VisAttributes::Invisible);
    row->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVReplica("fibreLayerRepFibre", fibre, row, kYAxis, fNumberOfStripsX, fFibre.getWidth());
    new G4PVReplica("fibreLayerRepFibre", row, layer, kXAxis, fNumberOfStripsY, fFibre.getWidth());

    return layer;
}

double FibreLayer::getSizeX()
{ 
    return fNumberOfStripsX * fFibre.getWidth();
}

double FibreLayer::getSizeY()
{
    return fNumberOfStripsY * fFibre.getLength();
}

} // namespace SiFi
