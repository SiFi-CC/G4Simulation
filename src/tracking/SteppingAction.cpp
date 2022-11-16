#include "SteppingAction.h"
#include <G4Step.hh>
#include <cmath>

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
        auto fiber_tmp = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(2);
        auto layer_tmp = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(3);
        if (fStorage->GetHmatrixEnable())
        {
            log->info("Hi Matrix");
        }
        if ((step->GetTrack()->GetParticleDefinition()->GetParticleType() == G4String("gamma")) |
            (fiber_tmp != fCurrentFiber) |
            (layer_tmp != fCurrentLayer))
        {
            if (fTotalDeposit > 0)
            {
                log->debug("total Deposit {}", fTotalDeposit);
                // log->debug("Scale {}", fSigmaScaleFactor);
                // double TotalDepositSmeared = fRandom->Gaus(
                //     0.0, sigma_energy_JK(fTotalDeposit));
                double sigma = 0.097*sigma_energy_JK(fTotalDeposit)/sigma_energy_JK(0.511);
                log->debug("sigma {}", sigma);
                double TotalDepositSmeared = fRandom->Gaus(0.0, fTotalDeposit * sigma);
                log->debug("total DepositError {} {}", fTotalDeposit, TotalDepositSmeared);
                log->debug("center of mass {} {} {}", fCenterOfMass.x(), fCenterOfMass.y(),
                    fCenterOfMass.z());
                fStorage->registerDepositScoring(
                    name, fCenterOfMass, fTotalDeposit + TotalDepositSmeared, TotalDepositSmeared);
            }
            fTotalDeposit = 0.0;
            fCurrentFiber = -1;
            fCurrentLayer = -1;
            fCenterOfMass = G4ThreeVector(0, 0, 0);
            log->debug("Event reset");
        }
        if (deposit > 0)
        {
            auto pos = step->GetPostStepPoint()->GetPosition();
            fCurrentFiber = fiber_tmp;
            fCurrentLayer = layer_tmp;
            log->debug("hit in fiber {} {} {}", fStorage->GetFiberNumber(pos.x()), fiber_tmp,
                       layer_tmp);
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

// Energy in MeV
Double_t SteppingAction::sigma_energy_JK(Double_t E)
{
    Double_t sigma = 0.0017 + 0.0355 * pow(E, -1. / 2.) - 0.00007 * pow(E, -3. / 2.);
    return sigma;
}

} // namespace SiFi
