#include "CrystalLayer.h"

#include "MaterialManager.h"
#include "Utils.h"
#include <G4Box.hh>
#include <G4PVReplica.hh>
#include <G4VisAttributes.hh>

namespace SiFi
{

G4LogicalVolume* CrystalLayer::Construct()
{
    auto row = new G4LogicalVolume(
        new G4Box("crystalRowSolid", getSizeX() / 2, fCrystal.getSizeY() / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "crystalRowLogical");

    auto layer = new G4LogicalVolume(
        new G4Box("crystalLayerSolid", getSizeX() / 2, getSizeY() / 2, getThickness() / 2),
        MaterialManager::get()->Vacuum(), "crystalLayerLogical");

    auto crystal = fCrystal.Construct();
    layer->SetVisAttributes(G4VisAttributes::Invisible);
    row->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVReplica("crystalLayerRepCrystal", crystal, row, kXAxis, fNumberOfStripsX, fCrystal.getSizeX());
    new G4PVReplica("crystalLayerRepCrystal", row, layer, kYAxis, fNumberOfStripsY, fCrystal.getSizeY());

    return layer;
}

double CrystalLayer::getSizeX()
{ 
    return fNumberOfStripsX * fCrystal.getSizeX();
}

double CrystalLayer::getSizeY()
{
    return fNumberOfStripsY * fCrystal.getSizeY();
}

double CrystalLayer::getThickness()
{
    return fCrystal.getThickness();
}

int CrystalLayer::getNumberOfStripsX()
{
    return fNumberOfStripsX;
}

int CrystalLayer::getNumberOfStripsY()
{
    return fNumberOfStripsY;
}

} // namespace SiFi
