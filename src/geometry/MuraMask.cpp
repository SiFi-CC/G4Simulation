#include "MuraMask.h"
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4VisAttributes.hh>
#include <TFile.h>
#include <TH2F.h>

namespace SiFi {

G4LogicalVolume* MuraMask::Construct() {
    // TODO: generate masks here
    auto file = new TFile("../../sificcsimulation/macros/hMURA2d_11.root", "READ");
    TH2F* muraHist = static_cast<TH2F*>(file->Get("hMURA2d"));

    auto mask = new G4LogicalVolume(
        new G4Box("mask", fSize.x() / 2, fSize.y() / 2, fSize.z() / 2),
        fMaterial,
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

    for (int i = 1; i < fMaskOrder + 1; i++) {
        for (int j = 1; j < fMaskOrder + 1; j++) {
            if (muraHist->GetBinContent(i, j) > 0) {
                auto posX =
                    muraHist->GetXaxis()->GetBinCenter(i) * segX - fMaskOrder * segX / 2;
                auto posY =
                    muraHist->GetYaxis()->GetBinCenter(j) * segY - fMaskOrder * segY / 2;

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

} // namespace SiFi
