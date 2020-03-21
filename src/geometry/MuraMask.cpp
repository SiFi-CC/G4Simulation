#include "MuraMask.h"
#include "MaterialManager.h"
#include "Utils.h"
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
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
    auto segX = fSize.x() / fMaskOrder;
    auto segY = fSize.y() / fMaskOrder;

    G4LogicalVolume* mask;


    if(fType == "standart"){
        mask = new G4LogicalVolume(
        new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
        MaterialManager::get()->Vacuum(),
        "mask");

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
    } else if (fType == "round"){
        mask = new G4LogicalVolume(
            new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
            fMaterial, "mask");

        auto maskSegment = new G4LogicalVolume(
            new G4Tubs("maskSegment", 0, segX / 2, 1.001*fSize.z() / 2, 0 * deg, 360 * deg),
            MaterialManager::get()->GetMaterial("G4_AIR"),
            "maskSegment");
        maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
        mask->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));
        G4LogicalVolume* maskSegmentTmp;

        int maskFlags[fMaskOrder][fMaskOrder]={0};

        int placementId = 1;
        int size = 1;
        for (int i = 0; i < fMaskOrder; i++) {
            for (int j = 0; j < fMaskOrder; j++) {
                maskFlags[i][j] = 0;
            }
        }

        for (int i = 0; i < fMaskOrder; i++) {
            for (int j = 0; j < fMaskOrder; j++) {
                if (!isMaskedAt(i, j) && maskFlags[i][j] == 0) {
                // if (!isMaskedAt(i, j)) {
                    maskFlags[i][j] = 1;
                    auto posX = (i + 0.5) * segX - fSize.x() / 2;
                    auto posY = (j + 0.5) * segY - fSize.y() / 2;
                    size = 1;
                    if(j < fMaskOrder-1 && i < fMaskOrder-1 && i != 0 && j != 0){
                        while (!isMaskedAt(i+size, j) && !isMaskedAt(i, j+size) && i+size < fMaskOrder && j+size < fMaskOrder){
                            size++;
                        }
                    }
                    if (size == 1){
                        new G4PVPlacement(
                            nullptr,
                            G4ThreeVector(posX, posY, 0),
                            maskSegment,
                            "maskBin",
                            mask,
                            false,
                            placementId);
                    } else {
                        for (int icount = i; icount < i+size; icount++){
                            for (int jcount = j; jcount < j+size; jcount++){
                                spdlog::info("Mask at {} {} size = {}", icount, jcount, size);
                                maskFlags[icount][jcount] = 1;
                            }
                        }
                        maskSegmentTmp = new G4LogicalVolume(
                            new G4Tubs("maskSegment", 0, size * segX / 2, 1.001*fSize.z() / 2, 0 * deg, 360 * deg),
                            MaterialManager::get()->GetMaterial("G4_AIR"),
                            "maskSegment");
                        maskSegmentTmp->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

                        new G4PVPlacement(
                            nullptr,
                            G4ThreeVector(posX + (size-1)*segX / 2, posY + (size-1)*segY / 2, 0),
                            maskSegmentTmp,
                            "maskBin",
                            mask,
                            false,
                            placementId);
                    }
                    placementId++;
                }
            }
        }
    } else if (fType == "round2"){
        mask = new G4LogicalVolume(
            new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
            fMaterial, "mask");

        auto maskSegment = new G4LogicalVolume(
            new G4Tubs("maskSegment", 0, segX / 2, 1.001*fSize.z() / 2, 0 * deg, 360 * deg),
            MaterialManager::get()->GetMaterial("G4_AIR"),
            "maskSegment");
        maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
        mask->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));
        G4LogicalVolume* maskSegmentTmp;

        int maskFlags[fMaskOrder][fMaskOrder]={0};

        int placementId = 1;
        int size = 1;
        for (int i = 0; i < fMaskOrder; i++) {
            for (int j = 0; j < fMaskOrder; j++) {
                maskFlags[i][j] = 0;
            }
        }
        for (int i = 0; i < fMaskOrder; i++) {
            for (int j = 0; j < fMaskOrder; j++) {
                // if (!isMaskedAt(i, j) && maskFlags[i][j] == 0) {
                if (!isMaskedAt(i, j)) {
                    // maskFlags[i][j] = 1;
                    auto posX = (i + 0.5) * segX - fSize.x() / 2;
                    auto posY = (j + 0.5) * segY - fSize.y() / 2;
                    size = 1;
                    if(j < fMaskOrder-1 && i < fMaskOrder-1 && i != 0 && j != 0){
                        while (!isMaskedAt(i+size, j) && !isMaskedAt(i, j+size) && i+size < fMaskOrder && j+size < fMaskOrder){
                            size++;
                        }
                    }
                    if(size >= maskFlags[i][j]){
                        for (int icount = i; icount < i+size; icount++){
                            for (int jcount = j; jcount < j+size; jcount++){
                                spdlog::info("Mask at {} {} size = {}", icount, jcount, size);
                                maskFlags[icount][jcount] = size;
                            }
                        }
                        maskSegmentTmp = new G4LogicalVolume(
                            new G4Tubs("maskSegment", 0, size * segX / 2, 1.001*fSize.z() / 2, 0 * deg, 360 * deg),
                            MaterialManager::get()->GetMaterial("G4_AIR"),
                            "maskSegment");
                        maskSegmentTmp->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

                        new G4PVPlacement(
                            nullptr,
                            G4ThreeVector(posX + (size-1)*segX / 2, posY + (size-1)*segY / 2, 0),
                            maskSegmentTmp,
                            "maskBin",
                            mask,
                            false,
                            placementId);
                    }
                    placementId++;
                }
            }
        }
    }  else {
        auto petmaterial = MaterialManager::get()->GetMaterial("G4_POLYETHYLENE");

        mask = new G4LogicalVolume(
            new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
            petmaterial, "mask");

        auto maskSegmentTung = new G4LogicalVolume(
            new G4Box("maskSegment", 0.9*segX / 2, 0.9*segY / 2, 1.001*fSize.z() / 2),
            fMaterial,"maskSegment");
        auto maskSegmentAir = new G4LogicalVolume(
            new G4Box("maskSegment", 0.9*segX / 2, 0.9*segY / 2, 1.001*fSize.z() / 2),
            MaterialManager::get()->GetMaterial("G4_AIR"), "maskSegment");
        maskSegmentTung->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
        maskSegmentAir->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
        // G4VisAttributes wire;
        // maskSegmentAir->SetVisAttributes(wire.setForceWireframe(true));
        mask->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));

        int placementId = 1;

        for (int i = 0; i < fMaskOrder; i++) {
            for (int j = 0; j < fMaskOrder; j++) {
                auto posX = (i + 0.5) * segX - fSize.x() / 2;
                auto posY = (j + 0.5) * segY - fSize.y() / 2;
                if (isMaskedAt(i, j)) {
                    new G4PVPlacement(
                        nullptr,
                        G4ThreeVector(posX, posY, 0),
                        maskSegmentTung,
                        "maskBin",
                        mask,
                        false,
                        placementId);
                } else {
                    new G4PVPlacement(
                        nullptr,
                        G4ThreeVector(posX, posY, 0),
                        maskSegmentAir,
                        "maskBin",
                        mask,
                        false,
                        placementId);
                }
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
