#include "Source.hh"
#include <TVector3.h>


namespace SiFi{

Source::Source(const Int_t& energy = 4400, const double& mintheta = 170, const double& maxtheta = 180)
:fEnergy(energy), fMinTheta(mintheta), fMaxTheta(maxtheta){
    Init(TVector3(0,0,0));
}


TVector3 Source::GetPos(){
	return fPosition;
}

void Source::Init(const TVector3& position){
    
    fPosition = position;
	fSourceGeant = new G4GeneralParticleSource;
    fSourceGeant->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMinTheta(fMinTheta * deg);
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMaxTheta(fMaxTheta * deg);
    fSourceGeant->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    fSourceGeant->GetCurrentSource()->GetPosDist()->SetCentreCoords(
        G4ThreeVector(fPosition.X() * cm, fPosition.Y() * cm, fPosition.Z() * cm));
    fSourceGeant->GetCurrentSource()->GetEneDist()->SetMonoEnergy(fEnergy * keV);
}

void Source::SetPos(const TVector3& position){
    fPosition = position;
    // fPosition = TVector2(2,1);
    fSourceGeant->GetCurrentSource()->GetPosDist()->SetCentreCoords(
        G4ThreeVector(fPosition.X() * cm, fPosition.Y() * cm, fPosition.Z() * cm));
}

}