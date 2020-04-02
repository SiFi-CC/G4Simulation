#include "SteppingAction.h"
#include <G4Step.hh>

namespace SiFi {

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto name = step->GetTrack()->GetVolume()->GetName();
    if (name != "world") {
        log->debug("hit in volume {}", name);
    }

    double deposit = step->GetTotalEnergyDeposit();
    if (deposit > 0 && name == "fibrephysical") {
        auto pos = step->GetPostStepPoint()->GetPosition();

        log->debug("hit in volume {}", name);

        fStorage->registerDepositScoring(name, pos, deposit);
        if (step->GetTrack()->GetParticleDefinition()->GetParticleType() == G4String("gamma")){
            // log->info("ID = {} ",step->GetTrack()->GetTrackID() );
            fStorage->gammacount();
        }
        // if(step->GetTrack()->GetTrackID() == 1){
        //     log->info("type = {} ",step->GetTrack()->GetParticleDefinition()->GetParticleType () );
        //     fStorage->gammacount();
        // }
    }
}

} // namespace SiFi
