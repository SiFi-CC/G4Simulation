#pragma once

#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"


#include <TVector2.h>

namespace SiFi {

class Source {

public:
	Source(const TVector2& position, const Int_t& energy, const double& mintheta, const double& maxtheta): 
			fPosition(position), fEnergy(energy), fMinTheta(mintheta), fMaxTheta(maxtheta){
				// Init();
			};
	TVector2 GetPos();

	void Init();

	G4GeneralParticleSource* GetSource(){return mSource;}

	Int_t GetEnergy(){return fEnergy;};
	double GetMinTheta(){return fMinTheta;};
	double GetMaxTheta(){return fMaxTheta;};

private:

	TVector2 fPosition;

	Int_t fEnergy;

	G4GeneralParticleSource* mSource = nullptr;

	double fMinTheta, fMaxTheta;
};


}