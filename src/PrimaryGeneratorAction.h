#pragma once

#include "Utils.h"
#include <Geant4/G4Event.hh>
#include <Geant4/G4GeneralParticleSource.hh>
#include <Geant4/G4ParticleDefinition.hh>
#include <Geant4/G4ParticleTable.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4ThreeVector.hh>
#include <Geant4/G4Types.hh>
#include <Geant4/G4VUserPrimaryGeneratorAction.hh>

namespace SiFi {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  public:
    explicit PrimaryGeneratorAction(G4GeneralParticleSource* gps) : fSource(gps) {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
        if (particle == nullptr) {
            log->error("Unable to find particle in particle table");
        }
        gps->SetParticleDefinition(particle);
        gps->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
        gps->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
        gps->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    };

    void GeneratePrimaries(G4Event* event) override {
        log->debug("GeneratePrimaries");
        fSource->GeneratePrimaryVertex(event);
        log->debug("GeneratePrimaries end");
    }
    const logger log = createLogger("PrimaryGeneratorAction");

  private:
    G4GeneralParticleSource* fSource;
};

} // namespace SiFi
