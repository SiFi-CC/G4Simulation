#include "MuraMask.h"
#include <Geant4/G4PVPlacement.hh>
#include <TFile.h>
#include <TH2F.h>

namespace SiFi {

MuraMask::MuraMask(int n, G4double x, G4double y, G4double z, G4Material* mat) {
    auto file = new TFile("../../sificcsimulation/macros/hMURA2d_11.root", "READ");
    TH2F* muraHist = static_cast<TH2F*>(file->Get("hMURA2d"));

    auto maskBox = new G4Box("mask", x / 2, y / 2, z / 2);
    fMaskLog = new G4LogicalVolume(maskBox, G4Material::GetMaterial("G4_W"), "mask");

    auto segX = x / n;
    auto segY = y / n;
    auto maskSegmentBox = new G4Box("maskSegment", segX / 2, segY / 2, z / 2);
    auto maskSegmentLog = new G4LogicalVolume(maskSegmentBox, mat, "maskSegment");

    int placementId = 1;

    for (int i = 1; i < n + 1; i++) {
        for (int j = 1; j < n + 1; j++) {
            if (muraHist->GetBinContent(i, j) > 0) {
                auto posX = muraHist->GetXaxis()->GetBinCenter(i) * segX - n * segX / 2;
                auto posY = muraHist->GetYaxis()->GetBinCenter(j) * segX - n * segY / 2;
                new G4PVPlacement(
                    nullptr, G4ThreeVector(posX, posY, 0), maskSegmentLog, "maskSegment", fMaskLog, false, placementId);
                placementId++;
            }
        }
    }
}

} // namespace SiFi
