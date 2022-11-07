#include "Source.hh"
#include <TVector3.h>
#include <math.h>

namespace SiFi
{

Source::Source(const Int_t& energy, const double& mintheta, const double& maxtheta)
    : fEnergy(energy), fMinTheta(mintheta), fMaxTheta(maxtheta)
{
    Init(TVector3(0, 0, 0));
}

TVector3 Source::GetPos() { return fPosition; }

void Source::Init(const TVector3& position)
{

    fPosition = position;
    fSourceGeant = new G4GeneralParticleSource;
    fSourceGeant->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMinTheta(fMinTheta * deg);
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMaxTheta(fMaxTheta * deg);
    fSourceGeant->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    fSourceGeant->GetCurrentSource()->GetPosDist()->SetCentreCoords(
        G4ThreeVector(fPosition.X() * mm, fPosition.Y() * mm, fPosition.Z() * mm));
    fSourceGeant->GetCurrentSource()->GetEneDist()->SetMonoEnergy(fEnergy * keV);
}

void Source::SetPosAng(const TVector3& position)
{
    fPosition = position;

    fSourceGeant->GetCurrentSource()->GetPosDist()->SetCentreCoords(
        G4ThreeVector(fPosition.X() * mm, fPosition.Y() * mm, fPosition.Z() * mm));
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMinTheta(fMinTheta * deg);
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMaxTheta(fMaxTheta * deg);
    // std::cout<<fSourceGeant->GetCurrentSource()->GetPosDist()->GetPosDisType()<<std::endl;

    // fSourceGeant->GetCurrentSource()->GetPosDist()->SetPosDisType("Beam");
    // if(1){
    //     fSourceGeant->GetCurrentSource()->GetPosDist()->SetPosDisShape("Rectangle");
    // 	fSourceGeant->GetCurrentSource()->GetPosDist()->SetHalfY(1 * mm);
    // 	// fSourceGeant->GetCurrentSource()->GetPosDist()->SetHalfY(5 * mm);
    // 	fSourceGeant->GetCurrentSource()->GetPosDist()->SetHalfX(15 * mm);
    // 	// fSourceGeant->GetCurrentSource()->GetPosDist()->SetHalfX(15 * mm);
    // 	// fSourceGeant->GetCurrentSource()->GetPosDist()->SetBeamSigmaInX(1 * mm);
    // 	// fSourceGeant->GetCurrentSource()->GetPosDist()->SetBeamSigmaInY(1 * mm);
    // } else {
    // fSourceGeant->GetCurrentSource()->GetPosDist()->SetPosDisShape("Circle");
    // 	fSourceGeant->GetCurrentSource()->GetPosDist()->SetRadius(1.0 * mm);
    // }
}

// Set the source rotated to the direction of the center of detector
// used only for system matrix calculation
void Source::SetPosAng(const TVector3& position, double det, double SD)
{
    fPosition = position;

    Double_t sx = fPosition.X();
    Double_t sy = fPosition.Y();
    Double_t R = sqrt(pow(SD, 2) + pow(sx, 2) + pow(sy, 2));

    G4ThreeVector xprime(1 - pow(sx, 2) / (pow(R, 2) + SD * R), -sx * sy / (pow(R, 2) + SD * R),
                         sx / R);
    G4ThreeVector yprime(xprime.getY(), 1 - pow(sy, 2) / (pow(R, 2) + SD * R), sy / R);

    SetAngles(det / 2.0, SD);
    fSourceGeant->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref1", xprime);
    fSourceGeant->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref2", yprime);

    fSourceGeant->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    fSourceGeant->GetCurrentSource()->GetPosDist()->SetCentreCoords(
        G4ThreeVector(fPosition.X() * mm, fPosition.Y() * mm, fPosition.Z() * mm));
//    fMinTheta = 165.0; 
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMinTheta(fMinTheta * deg);
    fSourceGeant->GetCurrentSource()->GetAngDist()->SetMaxTheta(fMaxTheta * deg);
}

void Source::SetAngles(const double det, const double SD)
{

    //double sx = fPosition.X();
    double sx = 0.0;
   // double sy = fPosition.Y();
    double sy = 0.0;

    double cos1 = (-sx * (det - sx) - sy * (det - sy) + pow(SD, 2)) /
                  sqrt(pow(sx, 2) + pow(sy, 2) + pow(SD, 2)) /
                  sqrt(pow(det - sx, 2) + pow(det - sy, 2) + pow(SD, 2));
    double cos2 = (-sx * (det - sx) - sy * (-det - sy) + pow(SD, 2)) /
                  sqrt(pow(sx, 2) + pow(sy, 2) + pow(SD, 2)) /
                  sqrt(pow(det - sx, 2) + pow(-det - sy, 2) + pow(SD, 2));
    double cos3 = (-sx * (-det - sx) - sy * (det - sy) + pow(SD, 2)) /
                  sqrt(pow(sx, 2) + pow(sy, 2) + pow(SD, 2)) /
                  sqrt(pow(-det - sx, 2) + pow(det - sy, 2) + pow(SD, 2));
    double cos4 = (-sx * (-det - sx) - sy * (-det - sy) + pow(SD, 2)) /
                  sqrt(pow(sx, 2) + pow(sy, 2) + pow(SD, 2)) /
                  sqrt(pow(-det - sx, 2) + pow(-det - sy, 2) + pow(SD, 2));

    fMinTheta = 180.0 - std::min({acos(cos1) * 180 / M_PI, acos(cos2) * 180 / M_PI,
                                  acos(cos3) * 180 / M_PI, acos(cos4) * 180 / M_PI});
}

} // namespace SiFi
