#include "SteppingAction.h"
#include <G4Step.hh>

namespace SiFi {

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto name = step->GetTrack()->GetVolume()->GetName();
    if (name != "world") {
        log->debug("hit in volume {}", name);
    }

    double deposit = step->GetTotalEnergyDeposit();
    if (deposit > 0) {
        auto pos = step->GetPostStepPoint()->GetPosition();

        log->debug("hit in volume {}", name);
        // log->info("dfs{}",1);


        fStorage->registerDepositScoring(name, pos, deposit);
    }
}

} // namespace SiFi
