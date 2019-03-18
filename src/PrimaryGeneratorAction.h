#pragma once

#include "Utils.h"
#include <Geant4/G4GeneralParticleSource.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4ThreeVector.hh>
#include <Geant4/G4Types.hh>
#include <Geant4/G4VUserPrimaryGeneratorAction.hh>

namespace SiFi {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  public:
    explicit PrimaryGeneratorAction(const G4String& particleName = "photon",
                                    G4double energy = 1. * MeV,
                                    const G4ThreeVector& position = G4ThreeVector(0, 0, 0),
                                    const G4ThreeVector& momentumDirection = G4ThreeVector(0,
                                                                                           0,
                                                                                           1));
    void GeneratePrimaries(G4Event* event) override;

    const logger log = createLogger("PrimaryGeneratorAction");

  private:
    G4GeneralParticleSource fSource;
};

} // namespace SiFi
