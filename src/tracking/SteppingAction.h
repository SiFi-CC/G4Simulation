#pragma once

#include "DataStorage.h"
#include "Utils.h"
#include <G4UserSteppingAction.hh>
#include <TVector3.h>

namespace SiFi
{

class SteppingAction : public G4UserSteppingAction
{
public:
    explicit SteppingAction(DataStorage* storage) : fStorage(storage){};

    void UserSteppingAction(const G4Step* step) override;

    const logger log = createLogger("Simulation");

private:
    DataStorage* fStorage = nullptr;

    G4ThreeVector fCenterOfMass =  G4ThreeVector(0, 0, 0);
    Double_t fTotalDeposit = 0.0;
};

} // namespace SiFi
