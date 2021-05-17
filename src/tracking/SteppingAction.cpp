#include "SteppingAction.h"
#include <G4Step.hh>

namespace SiFi {

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto name = step->GetTrack()->GetVolume()->GetName();
    if (name != "world") {
        log->debug("hit in volume {}", name);
    }
    parentID = step->GetTrack()->GetParentID();
    if (parentID==0){
            fParentPos = step->GetTrack()->GetVertexPosition();
            fParentEn = step->GetTrack()->GetVertexKineticEnergy();
        }

    double deposit = step->GetTotalEnergyDeposit();
    if (name == "fibrephysical") {
        auto pos = step->GetPostStepPoint()->GetPosition();
        auto dir = step->GetPreStepPoint()-> GetMomentumDirection();
        // auto dir = step->GetTrack()->GetVertexMomentumDirection();
        auto camefrom = step->GetTrack()->GetVertexPosition();

        log->debug("hit in volume {}", name);   

        fStorage->registerDepositScoring(name, pos, deposit, dir, fParentPos, camefrom, fParentEn,
        step->GetTrack()->GetParticleDefinition()->GetParticleName(), parentID);
        // log->info("*****Source {} {} {} {} {} {}", step->GetTrack()->GetVertexPosition().x(), step->GetTrack()->GetVertexPosition().y(), step->GetTrack()->GetVertexPosition().z(), step->GetTrack()->GetVertexKineticEnergy(), deposit,  step->GetTrack()->GetParticleDefinition()->GetParticleType());
        // log->info("*****Parent {} {} {} {} {} {}", fParentPos.getX(), fParentPos.getY(), fParentPos.getZ(), fParentEn, deposit, step->GetTrack()->GetParticleDefinition()->GetParticleType(), step->GetTrack()->GetParticleDefinition()->GetParticleType());
        if (step->GetTrack()->GetParticleDefinition()->GetParticleType() == G4String("gamma")){
            fStorage->gammacount();
        }
    }
}

} // namespace SiFi
