#include "FibreLayer_Scatterrer.h"

#include "MaterialManager.h"
#include "Utils.h"
#include <G4Box.hh>
#include <G4PVReplica.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

G4LogicalVolume* FibreLayer_Scatterrer::Construct()
{
    //changed X and Y - I do not know why it works
    auto layer = new G4LogicalVolume(
        new G4Box("fibreLayerSolid", getSizeY() / 2, 110.6 / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "fibreLayerLogical");
    auto largestack = new G4LogicalVolume(
        new G4Box("fibreLayerSolid", getSizeY() / 2, 17 / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "largestackLogical");
    auto smallstack = new G4LogicalVolume(
        new G4Box("fibreLayerSolid", getSizeY() / 2, 14 / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "smallstackLogical");
    
    auto fibre = fFibre.Construct();

    new G4PVReplica("largeStackRepFibre", fibre, largestack, kYAxis, 8, fFibre.getWidth());
    new G4PVReplica("smallstackRepFibre", fibre, smallstack, kYAxis, 7, fFibre.getWidth());
    
    double offset = 0.1;
    double total_width = 6 * 8 * fFibre.getWidth() + 7 * fFibre.getWidth() + offset * 6;

    layer->SetVisAttributes(G4VisAttributes::Invisible);
    for (int i = 0; i < 6; i++)
    {
        new G4PVPlacement(
                    0, G4ThreeVector(0,
                    -total_width/2 + (8 * fFibre.getWidth() + offset) *i + 8 * fFibre.getWidth()/2.0, 0),
                    largestack, "largestack", layer, 0, i, 0);
        spdlog::debug("LargeStack{} [{}, {}]", i, -total_width/2 + (8 * fFibre.getWidth() + offset) *i,
                                                -total_width/2 + (8 * fFibre.getWidth() + offset) *(i+1) -offset);
    }
    new G4PVPlacement(
                    0, G4ThreeVector(0, total_width/2 - 7 * fFibre.getWidth() / 2.0, 0),
                    smallstack, "smallstack", layer, 0, 6, 0);
    // new G4PVPlacement(
    //             0, G4ThreeVector(0, 20 * mm, 0),
    //             largestack, "blahblah", layer, 0, 1, 0);
    // new G4PVReplica("fibreLayerRepFibre", largestack, layer, kYAxis, 6, 17 * mm);

    return layer;
}

double FibreLayer_Scatterrer::getSizeX()
{ 
    return fNumberOfStrips * fFibre.getWidth();
}

double FibreLayer_Scatterrer::getSizeY()
{
    return fFibre.getLength();
}

} // namespace SiFi
