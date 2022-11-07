#include "MuraMask.h"
#include "MaterialManager.h"
#include "Utils.h"
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TRandom3.h>

#include "CmdLineConfig.hh"

namespace SiFi
{

bool MuraMask::isMaskedAt(int x, int y)
{
    if (x == 0) { return false; }
    if (y == 0) { return true; }
    if (isQuaResidue(x, fMaskOrder) * isQuaResidue(y, fMaskOrder) == -1) { return true; }
    return false;
}

bool MuraMask::isMaskedAt(int x)
{
    if (x == 0) { return false; }
    if (isQuaResidue(x, fMaskOrder) == -1) { return true; }
    return false;
}

int MuraMask::isQuaResidue(int q, int p)
{
    int result = -1;
    for (int i = 1; i < p; i++)
    {
        if ((i * i) % p == q)
        {
            result = 1;
            break;
        }
    }
    return result;
}

G4LogicalVolume* MuraMask::Construct()
{
    auto segX = fSize.x() / fMaskOrder;
    auto segY = fSize.y() / fMaskOrder;

    G4LogicalVolume* mask = NULL;

    bool ismask = false;

    if (fType == "standart")
    {
        mask = new G4LogicalVolume(new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
                                   MaterialManager::get()->GetMaterial("G4_AIR"), "mask");

        auto maskSegment = new G4LogicalVolume(
            new G4Box("maskSegment", segX / 2, segY / 2, fSize.z() / 2), fMaterial, "maskSegment");
        maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
        mask->SetVisAttributes(G4VisAttributes::Invisible);

        int placementId = 1;

        for (int i = 0; i < fMaskOrder; i++)
        {
            for (int j = 0; j < fMaskOrder; j++)
            {
                if (CmdLineOption::GetFlagValue("Single_dimension")) { ismask = isMaskedAt(i); }
                else
                {
                    ismask = isMaskedAt(i, j);
                }
                if (ismask)
                {
                    auto posX = (i + 0.5) * segX - fSize.x() / 2;
                    auto posY = (j + 0.5) * segY - fSize.y() / 2;

                    new G4PVPlacement(nullptr, G4ThreeVector(posX, posY, 0), maskSegment, "maskBin",
                                      mask, false, placementId);
                    placementId++;
                }
            }
        }
    }
    else if (fType == "round")
    {
        mask = new G4LogicalVolume(new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
                                   fMaterial, "mask");

        auto maskSegment = new G4LogicalVolume(
            new G4Tubs("maskSegment", 0, segX / 2, 1.001 * fSize.z() / 2, 0 * deg, 360 * deg),
            MaterialManager::get()->GetMaterial("G4_AIR"), "maskSegment");
        maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
        mask->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));
        G4LogicalVolume* maskSegmentTmp;

        std::vector<std::vector<int>> maskFlags(fMaskOrder, std::vector<int>(fMaskOrder, 0));

        int placementId = 1;
        int size = 1;
        for (int i = 0; i < fMaskOrder; i++)
        {
            for (int j = 0; j < fMaskOrder; j++)
            {
                maskFlags[i][j] = 0;
            }
        }
        for (int i = 0; i < fMaskOrder; i++)
        {
            for (int j = 0; j < fMaskOrder; j++)
            {
                if (!isMaskedAt(i, j))
                {
                    auto posX = (i + 0.5) * segX - fSize.x() / 2;
                    auto posY = (j + 0.5) * segY - fSize.y() / 2;
                    size = 1;
                    if (j < fMaskOrder - 1 && i < fMaskOrder - 1 && i != 0 && j != 0)
                    {
                        while (!isMaskedAt(i + size, j) && !isMaskedAt(i, j + size) &&
                               i + size < fMaskOrder && j + size < fMaskOrder)
                        {
                            size++;
                        }
                    }
                    if (size >= maskFlags[i][j])
                    {
                        for (int icount = i; icount < i + size; icount++)
                        {
                            for (int jcount = j; jcount < j + size; jcount++)
                            {
                                maskFlags[icount][jcount] = size;
                            }
                        }
                        maskSegmentTmp = new G4LogicalVolume(
                            new G4Tubs("maskSegment", 0, size * segX / 2, 1.001 * fSize.z() / 2,
                                       0 * deg, 360 * deg),
                            MaterialManager::get()->GetMaterial("G4_AIR"), "maskSegment");
                        maskSegmentTmp->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

                        new G4PVPlacement(nullptr,
                                          G4ThreeVector(posX + (size - 1) * segX / 2,
                                                        posY + (size - 1) * segY / 2, 0),
                                          maskSegmentTmp, "maskBin", mask, false, placementId);
                    }
                    placementId++;
                }
            }
        }
    }
    else if (fType == "nowallpet")
    {
        auto gRandom = new TRandom3();
        gRandom->SetSeed(0);
        double deltaX, deltaY, deltaZ;
        double error = CmdLineOption::GetDoubleValue("Precision");
        TH1D *histX, *histY, *histZ;

        histX = new TH1D("rodsX", "X Size of Rods", 100, segX - 0.03 - error, segX + 0.03);
        histY = new TH1D("rodsY", "Y Size of Rods", 100, segY - 0.03 - error, segY + 0.03);
        histZ =
            new TH1D("rodsZ", "Z Size of Rods", 100, fSize.z() - 0.03 - error, fSize.z() + 0.03);

        auto petmaterial = MaterialManager::get()->GetMaterial("G4_POLYETHYLENE");
        mask = new G4LogicalVolume(
            new G4Box("mask", 1.01 * fSize.x() / 2, 1.01 * fSize.y() / 2, fSize.z() / 4),
            petmaterial, "mask");

        auto maskSegment = new G4LogicalVolume(
            new G4Box("maskSegment", segX / 2, segY / 2, fSize.z() / 2), fMaterial, "maskSegment");
        maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
        mask->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));

        int placementId = 1;

        for (int i = 0; i < fMaskOrder; i++)
        {
            for (int j = 0; j < fMaskOrder; j++)
            {
                if (CmdLineOption::GetFlagValue("Single_dimension")) { ismask = isMaskedAt(i); }
                else
                {
                    ismask = isMaskedAt(i, j);
                }
                if (ismask)
                {
                    auto posX = (i + 0.5) * segX - fSize.x() / 2;
                    auto posY = (j + 0.5) * segY - fSize.y() / 2;

                    // if (error > 0){
                    deltaX = gRandom->Gaus(error / 2.0, error / 6.0);
                    histX->Fill(segX - deltaX);
                    deltaY = gRandom->Gaus(error / 2.0, error / 6.0);
                    histY->Fill(segY - deltaY);
                    deltaZ = gRandom->Gaus(error / 2.0, error / 6.0);
                    histZ->Fill(fSize.z() - deltaZ);

                    maskSegment = new G4LogicalVolume(new G4Box("maskSegment", (segX - deltaX) / 2,
                                                                (segY - deltaY) / 2,
                                                                (fSize.z() - deltaZ) / 2),
                                                      fMaterial, "maskSegment");
                    maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
                    // }

                    new G4PVPlacement(nullptr, G4ThreeVector(posX, posY, 0), maskSegment, "maskBin",
                                      mask, false, placementId);
                    placementId++;
                }
            }
        }
    }
    else if (fType == "nowallpetcut")
    {
        int cut = CmdLineOption::GetIntValue("MaskCut");
        int cutx = CmdLineOption::GetIntValue("MaskCutX");
        int cuty = CmdLineOption::GetIntValue("MaskCutY");
        cutx = (cutx == 0) ? cut : cutx;
        cuty = (cuty == 0) ? cut : cuty;
        
        segX = fSize.x() / cutx;
        segY = fSize.y() / cuty;
        auto gRandom = new TRandom3();
        gRandom->SetSeed(0);
        double deltaX, deltaY, deltaZ;
        double error = CmdLineOption::GetDoubleValue("Precision");
        TH1D *histX, *histY, *histZ;

        histX = new TH1D("rodsX", "X Size of Rods", 100, segX - 0.03 - error, segX + 0.03);
        histY = new TH1D("rodsY", "Y Size of Rods", 100, segY - 0.03 - error, segY + 0.03);
        histZ =
            new TH1D("rodsZ", "Z Size of Rods", 100, fSize.z() - 0.03 - error, fSize.z() + 0.03);

        auto petmaterial = MaterialManager::get()->GetMaterial("G4_POLYETHYLENE");
        mask = new G4LogicalVolume(
            new G4Box("mask", 1.01 * fSize.x() / 2, 1.01 * fSize.y() / 2, fSize.z() / 4),
            petmaterial, "mask");

        auto maskSegment = new G4LogicalVolume(
            new G4Box("maskSegment", segX / 2, segY / 2, fSize.z() / 2), fMaterial, "maskSegment");
        maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
        mask->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));

        int placementId = 1;

        for (int i = 0; i < cutx; i++)
        {
            for (int j = 0; j < cuty; j++)
            {
                if (CmdLineOption::GetFlagValue("Single_dimension"))
                {
                    ismask = isMaskedAt(round(i + fMaskOrder / 2) - (int)cutx / 2);
                }
                else
                {
                    ismask = (isMaskedAt(round(i + fMaskOrder / 2) - (int)cutx / 2,
                                        j + round(fMaskOrder / 2) - (int)cuty / 2) ||
                             j == 0) && i != 0;
                }
                if (j==0)
                {
                    spdlog::info("MASK at {} {}", i , ismask);
                }
                if (ismask)
                {
                    auto posX = (i + 0.5) * segX - fSize.x() / 2;
                    auto posY = (j + 0.5) * segY - fSize.y() / 2;

                    // if (error > 0){
                    deltaX = gRandom->Gaus(error / 2.0, error / 6.0);
                    histX->Fill(segX - deltaX);
                    deltaY = gRandom->Gaus(error / 2.0, error / 6.0);
                    histY->Fill(segY - deltaY);
                    deltaZ = gRandom->Gaus(error / 2.0, error / 6.0);
                    histZ->Fill(fSize.z() - deltaZ);

                    maskSegment = new G4LogicalVolume(new G4Box("maskSegment", (segX - deltaX) / 2,
                                                                (segY - deltaY) / 2,
                                                                (fSize.z() - deltaZ) / 2),
                                                      fMaterial, "maskSegment");
                    maskSegment->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
                    // }

                    new G4PVPlacement(nullptr, G4ThreeVector(posX, posY, 0), maskSegment, "maskBin",
                                      mask, false, placementId);
                    placementId++;
                }
            }
        }
    }
    else if (fType == "pet")
    {
        auto gRandom = new TRandom3();
        gRandom->SetSeed(0);
        double deltaX, deltaY, deltaZ;
        double error = CmdLineOption::GetDoubleValue("Precision");
        TH1D *histX, *histY, *histZ;

        histX =
            new TH1D("rodsX", "X Size of Rods", 100, 0.9 * segX - 0.03 - error, 0.9 * segX + 0.03);
        histY =
            new TH1D("rodsY", "Y Size of Rods", 100, 0.9 * segY - 0.03 - error, 0.9 * segY + 0.03);
        histZ = new TH1D("rodsZ", "Z Size of Rods", 100, 1.001 * fSize.z() - 0.03 - error,
                         1.001 * fSize.z() + 0.03);

        auto petmaterial = MaterialManager::get()->GetMaterial("G4_POLYETHYLENE");

        mask = new G4LogicalVolume(new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
                                   petmaterial, "mask");

        auto maskSegmentTung = new G4LogicalVolume(
            new G4Box("maskSegment", 0.9 * segX / 2, 0.9 * segY / 2, 1.001 * fSize.z() / 2),
            fMaterial, "maskSegment");
        auto maskSegmentAir = new G4LogicalVolume(
            new G4Box("maskSegment", 0.9 * segX / 2, 0.9 * segY / 2, 1.001 * fSize.z() / 2),
            MaterialManager::get()->GetMaterial("G4_AIR"), "maskSegment");
        maskSegmentTung->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
        maskSegmentAir->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
        // G4VisAttributes wire;
        // maskSegmentAir->SetVisAttributes(wire.setForceWireframe(true));
        mask->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));

        int placementId = 1;

        for (int i = 0; i < fMaskOrder; i++)
        {
            for (int j = 0; j < fMaskOrder; j++)
            {
                auto posX = (i + 0.5) * segX - fSize.x() / 2;
                auto posY = (j + 0.5) * segY - fSize.y() / 2;
                if (CmdLineOption::GetFlagValue("Single_dimension")) { ismask = isMaskedAt(i); }
                else
                {
                    ismask = isMaskedAt(i, j);
                }
                if (ismask)
                {
                    // if (error > 0){
                    deltaX = gRandom->Gaus(0.05, 0.1 / 6.0);
                    histX->Fill(0.9 * segX - deltaX);
                    deltaY = gRandom->Gaus(0.05, 0.1 / 6.0);
                    histY->Fill(0.9 * segY - deltaY);
                    deltaZ = gRandom->Gaus(0.05, 0.1 / 6.0);
                    histZ->Fill(1.001 * fSize.z() - deltaZ);

                    maskSegmentTung = new G4LogicalVolume(
                        new G4Box("maskSegmentTung", (segX - deltaX) / 2, (segY - deltaY) / 2,
                                  (fSize.z() - deltaZ) / 2),
                        fMaterial, "maskSegmentTung");
                    maskSegmentTung = new G4LogicalVolume(
                        new G4Box("maskSegment", (0.9 * segX - deltaX) / 2,
                                  (0.9 * segY - deltaY) / 2, (1.001 * fSize.z() - deltaZ) / 2),
                        fMaterial, "maskSegment");
                    maskSegmentTung->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
                    // }
                    new G4PVPlacement(nullptr, G4ThreeVector(posX, posY, 0), maskSegmentTung,
                                      "maskBin", mask, false, placementId);
                }
                else
                {
                    new G4PVPlacement(nullptr, G4ThreeVector(posX, posY, 0), maskSegmentAir,
                                      "maskBin", mask, false, placementId);
                }
                placementId++;
            }
        }
    }

    return mask;
}

void MuraMask::writeMetadata(DataStorage* storage)
{
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
