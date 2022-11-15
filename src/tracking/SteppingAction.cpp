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
    if (name == "fibrephysical")
    {
        auto number = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(2);
        auto laynumber = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(3);
        if (fStorage->GetHmatrixEnable())
        {
            log->info("Hi Matrix");
        }
        if (step->GetTrack()->GetParticleDefinition()->GetParticleType() == G4String("gamma"))
        {
            if (fTotalDeposit > 0)
            {
                log->debug("total Deposit {}", fTotalDeposit);
                log->debug("center of mass {} {} {}", fCenterOfMass.x(), fCenterOfMass.y(),
                    fCenterOfMass.z());
                fStorage->registerDepositScoring(name, fCenterOfMass, fTotalDeposit);
            }
            fTotalDeposit = 0.0;
            fCenterOfMass = G4ThreeVector(0, 0, 0);
        }
        if (deposit > 0)
        {
            auto pos = step->GetPostStepPoint()->GetPosition();
            // log->debug("hit in fiber {}", fStorage->GetFiberNumber(pos.x()));
            // log->debug("hit in fiber {} {}", fStorage->GetFiberNumber(pos.x()), number);
            log->debug("hit in fiber {} {} {}", fStorage->GetFiberNumber(pos.x()), number,
                       laynumber);
            log->debug("Position {} {} {}", pos.x(), pos.y(), pos.z());
            log->debug("EnergyDeposit {}", deposit);
            if (fTotalDeposit == 0){
                fStorage->gammacount();
            }
            fCenterOfMass =
                1.0 / (fTotalDeposit + deposit) * (fTotalDeposit * fCenterOfMass + deposit * pos);
            fTotalDeposit += deposit;
        }
    }
}

} // namespace SiFi
