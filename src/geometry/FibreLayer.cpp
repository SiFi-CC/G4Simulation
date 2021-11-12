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
    auto layer = new G4LogicalVolume(
        new G4Box("fibreLayerSolid", getSizeY() / 2, getSizeX() / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "fibreLayerLogical");

    auto fibre = fFibre.Construct();
    layer->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVReplica("fibreLayerRepFibre", fibre, layer, kYAxis, fNumberOfStrips, fFibre.getWidth());

    return layer;
}

double FibreLayer::getSizeX()
{ 
    return fNumberOfStrips * fFibre.getWidth();
}

double FibreLayer::getSizeY()
{
    if (CmdLineOption::GetFlagValue("Single_dimension"))
    {
        return fFibre.getLength();
    }
    else
    {
        return fNumberOfStrips * fFibre.getWidth();
    }
}

} // namespace SiFi
