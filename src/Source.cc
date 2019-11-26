#include "Source.hh"
#include <TVector2.h>


namespace SiFi{

TVector2 Source::GetPos(){
	return fPosition;
}

void Source::Init(){
	mSource = new G4GeneralParticleSource;
    mSource->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    mSource->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
    mSource->GetCurrentSource()->GetAngDist()->SetMinTheta(fMinTheta * deg);
    mSource->GetCurrentSource()->GetAngDist()->SetMaxTheta(fMaxTheta * deg);
    mSource->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    mSource->GetCurrentSource()->GetPosDist()->SetCentreCoords(
        G4ThreeVector(fPosition.X() * cm, fPosition.Y() * cm, 0));
    mSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(fEnergy * keV);
}

}