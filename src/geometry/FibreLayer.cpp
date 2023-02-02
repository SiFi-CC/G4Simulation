#include "FibreLayer.h"

#include "MaterialManager.h"
#include "Utils.h"
#include <G4Box.hh>
#include <G4PVReplica.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

G4LogicalVolume* FibreLayer::Construct()
{   
    if (CmdLineOption::GetFlagValue("Full_scatterrer")){
        double offset = 0.1;
        double total_width = 6 * 8 * fFibre.getWidth() + 7 * fFibre.getWidth() + offset * 6;
        spdlog::info("Full-scale prototype {}", getSizeY());
        auto layer = new G4LogicalVolume(
            new G4Box("fibreLayerSolid", getSizeY() / 2, total_width / 2, getThickness() / 2),
            MaterialManager::get()->Vacuum(), "fibreLayerLogical");
        auto largestack = new G4LogicalVolume(
            new G4Box("fibreLayerSolid", getSizeY() / 2, 8 * fFibre.getWidth() / 2, getThickness() / 2),
            MaterialManager::get()->Vacuum(), "largestackLogical");
        auto smallstack = new G4LogicalVolume(
            new G4Box("fibreLayerSolid", getSizeY() / 2, 7 * fFibre.getWidth() / 2, getThickness() / 2),
            MaterialManager::get()->Vacuum(), "smallstackLogical");
        
        auto fibre = fFibre.Construct();
        spdlog::info("FibreWidth = {}", fFibre.getWidth());

        new G4PVReplica("largeStackRepFibre", fibre, largestack, kYAxis, 8, fFibre.getWidth());
        new G4PVReplica("smallstackRepFibre", fibre, smallstack, kYAxis, 7, fFibre.getWidth());

        layer->SetVisAttributes(G4VisAttributes::Invisible);
        for (int i = 0; i < 6; i++)
        {
            new G4PVPlacement(
                        0, G4ThreeVector(0,
                        -total_width/2 + (8 * fFibre.getWidth() + offset) *i + 8 * fFibre.getWidth()/2.0, 0),
                        largestack, "largestack", layer, 0, i, 0);
            spdlog::info("LargeStack{} x = {}", i, -total_width/2 + (8 * fFibre.getWidth() + offset) *i + 8 * fFibre.getWidth()/2.0);
        }
        new G4PVPlacement(
                        0, G4ThreeVector(0, total_width/2 - 7 * fFibre.getWidth() / 2.0, 0),
                        smallstack, "smallstack", layer, 0, 6, 0);
        spdlog::info("SmallStack x = {}", total_width/2 - 7 * fFibre.getWidth() / 2.0);

        return layer;
    }
    else
    {
        //changed X and Y - I do not know why it works
        spdlog::info("Small-scale prototype {}", getSizeY());
        auto layer = new G4LogicalVolume(
            new G4Box("fibreLayerSolid", getSizeY() / 2, getSizeX() / 2, getThickness() / 2),
            MaterialManager::get()->Vacuum(), "fibreLayerLogical");

        auto fibre = fFibre.Construct();
        layer->SetVisAttributes(G4VisAttributes::Invisible);
        new G4PVReplica("fibreLayerRepFibre", fibre, layer, kYAxis, fNumberOfStrips, fFibre.getWidth());
        return layer;
    }

}

double FibreLayer::getSizeX()
{ 
    return fNumberOfStrips * fFibre.getWidth();
}

double FibreLayer::getSizeY()
{
    // if (CmdLineOption::GetFlagValue("Single_dimension"))
    // {
    return fFibre.getLength();
    // }
    // else
    // {
    //     return fNumberOfStrips * fFibre.getWidth();
    // }
}

} // namespace SiFi
