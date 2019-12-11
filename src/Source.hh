#pragma once

#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"


#include <TVector3.h>

namespace SiFi {

class Source {

public:
	Source(const Int_t& energy, const double& mintheta, const double& maxtheta);
	TVector3 GetPos();

	void Init(const TVector3& position = TVector3(0,0,0));
	void SetPos(const TVector3& position);

	G4GeneralParticleSource* GetSource(){return fSourceGeant;}

	Int_t GetEnergy(){return fEnergy;};
	double GetMinTheta(){return fMinTheta;};
	double GetMaxTheta(){return fMaxTheta;};

private:

	TVector3 fPosition;
	Int_t fEnergy;
	G4GeneralParticleSource* fSourceGeant = nullptr;
	double fMinTheta, fMaxTheta;
};


}