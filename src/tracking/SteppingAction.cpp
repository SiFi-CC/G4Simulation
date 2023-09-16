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
        auto pos = step->GetPreStepPoint()->GetPosition();
        // Only for -fullscat
        auto fiber_tmp = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(2);
        auto stack_tmp = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(3);
        auto layer_tmp = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(4);

        log->debug("hit in volume {}", name);
        log->debug("Position {} {} {}", pos.x(), pos.y(), pos.z());
        log->debug("Fiber {}, stack {}, layer {}", fiber_tmp, stack_tmp, layer_tmp);
        log->debug("FiberID {}", 8*stack_tmp + fiber_tmp);

        fStorage->registerDepositScoring(name, pos, deposit, 8*stack_tmp + fiber_tmp);
        if (step->GetTrack()->GetParticleDefinition()->GetParticleType() == G4String("gamma"))
        {
            fStorage->gammacount();
        }
    }
}

} // namespace SiFi
