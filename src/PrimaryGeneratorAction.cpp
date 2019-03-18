
#include "PrimaryGeneratorAction.h"

#include <Geant4/G4Event.hh>
#include <Geant4/G4ParticleDefinition.hh>
#include <Geant4/G4ParticleTable.hh>

namespace SiFi {

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4String& particleName,
                                               G4double energy,
                                               const G4ThreeVector& position,
                                               const G4ThreeVector& momentumDirection) {

    log->info("constructor particle type: {}, position: ({}, {}, {})",
              particleName,
              position.x(),
              position.y(),
              position.z());
    // default particle kinematic
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
    log->info("constructor end");
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    log->debug("GeneratePrimaries");
    fSource.GeneratePrimaryVertex(event);
    log->debug("GeneratePrimaries end");
}

} // namespace SiFi
