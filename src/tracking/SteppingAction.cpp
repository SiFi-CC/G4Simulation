#include "SteppingAction.h"
#include <G4Step.hh>

namespace SiFi
{

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto name = step->GetTrack()->GetVolume()->GetName();
    if (name != "world") { log->debug("hit in volume {}", name); }

    double deposit = step->GetTotalEnergyDeposit();
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
