#pragma once

#include "DataStorage.h"
#include "Utils.h"
#include <G4UserSteppingAction.hh>
#include <TRandom3.h>
#include <TVector3.h>

namespace SiFi
{

class SteppingAction : public G4UserSteppingAction
{
public:
    explicit SteppingAction(DataStorage* storage) : fStorage(storage){};

    void UserSteppingAction(const G4Step* step) override;

    const logger log = createLogger("Simulation");

    Double_t sigma_energy_JK(Double_t E);

private:

    DataStorage* fStorage = nullptr;

    G4ThreeVector fCenterOfMass =  G4ThreeVector(0, 0, 0);
    Double_t fTotalDeposit = 0.0;
    // Double_t fSigmaScaleFactor = 0.0;
    Int_t fCurrentFiber = -1;
    Int_t fCurrentLayer = -1;
    TRandom3* fRandom = new TRandom3();
};

} // namespace SiFi
