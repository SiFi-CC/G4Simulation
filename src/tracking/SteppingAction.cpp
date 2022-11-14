#include "SteppingAction.h"
#include <G4Step.hh>

namespace SiFi
{

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto name = step->GetTrack()->GetVolume()->GetName();
    double deposit = step->GetTotalEnergyDeposit();

    if (deposit > 0 && name == "crystalphysical")
    {
        log->debug("hit in volume {}", name);
        G4TouchableHandle theTouchable = step->GetPreStepPoint()->GetTouchableHandle();
        // layer number
        G4int grandgrandmotherCopyNo = theTouchable->GetReplicaNumber(3);
        log->debug("Layer number {}", grandgrandmotherCopyNo);

        auto pos = step->GetPostStepPoint()->GetPosition();

        fStorage->registerDepositScoringHypMed(name, grandgrandmotherCopyNo, pos, deposit);
        if (step->GetTrack()->GetParticleDefinition()->GetParticleType() == G4String("gamma"))
        {
            fStorage->gammacount();
        }
    }

    if (deposit > 0 && name == "fibrephysical")
    {
        auto pos = step->GetPostStepPoint()->GetPosition();

        log->debug("hit in volume {}", name);
        log->debug("Position {} {}", pos.x(), pos.y());

        fStorage->registerDepositScoring(name, pos, deposit);
        if (step->GetTrack()->GetParticleDefinition()->GetParticleType() == G4String("gamma"))
        {
            fStorage->gammacount();
        }
    }
}

} // namespace SiFi
