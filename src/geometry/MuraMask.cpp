#include "MuraMask.h"
#include "MaterialManager.h"
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4VisAttributes.hh>
#include <TFile.h>
#include <TH2F.h>

namespace SiFi {

bool MuraMask::isMaskedAt(int x, int y) {
    if (x == 0) {
        return false;
    }
    if (y == 0) {
        return true;
    }
    if (isQuaResidue(x, fMaskOrder) * isQuaResidue(y, fMaskOrder) == -1) {
        return true;
    }
    return false;
}

int MuraMask::isQuaResidue(int q, int p) {
    int result = -1;
    for (int i = 1; i < p; i++) {
        if ((i * i) % p == q) {
            result = 1;
            break;
        }
    }
    return result;
}

G4LogicalVolume* MuraMask::Construct() {
    auto mask = new G4LogicalVolume(
        new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
        MaterialManager::get()->Vacuum(),
        "mask");

    auto segX = fSize.x() / fMaskOrder;
    auto segY = fSize.y() / fMaskOrder;
    auto maskSegment = new G4LogicalVolume(
        new G4Box("maskSegment", segX / 2, segY / 2, fSize.z() / 2),
        fMaterial,
        "maskSegment");
    maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
    mask->SetVisAttributes(G4VisAttributes::Invisible);

    int placementId = 1;

    for (int i = 0; i < fMaskOrder; i++) {
        for (int j = 0; j < fMaskOrder; j++) {
            if (isMaskedAt(i, j)) {
                auto posX = (i + 0.5) * segX - fSize.x() / 2;
                auto posY = (j + 0.5) * segY - fSize.y() / 2;

                new G4PVPlacement(
                    nullptr,
                    G4ThreeVector(posX, posY, 0),
                    maskSegment,
                    "maskBin",
                    mask,
                    false,
                    placementId);
                placementId++;
            }
        }
    }
    return mask;
}

void MuraMask::writeMetadata(DataStorage* storage) {
    double zPosition = storage->getMetadataNumber("sourceToMaskDistance");
    storage->writeMetadata("maskMinX", -fSize.x() / 2);
    storage->writeMetadata("maskMaxX", fSize.x() / 2);
    storage->writeMetadata("maskMinY", -fSize.y() / 2);
    storage->writeMetadata("maskMaxY", fSize.y() / 2);
    storage->writeMetadata("maskMinZ", zPosition - fSize.z() / 2);
    storage->writeMetadata("maskMaxZ", zPosition + fSize.z() / 2);
    storage->writeMetadata("maskBinX", fMaskOrder);
    storage->writeMetadata("maskBinY", fMaskOrder);
}

} // namespace SiFi
