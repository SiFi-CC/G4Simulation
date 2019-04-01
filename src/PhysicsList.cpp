#include "PhysicsList.h"
#include "Utils.h"
#include <Geant4/G4ChargeExchangePhysics.hh>
#include <Geant4/G4ComptonScattering.hh>
#include <Geant4/G4DecayPhysics.hh>
#include <Geant4/G4EmExtraPhysics.hh>
#include <Geant4/G4EmLowEPPhysics.hh>
#include <Geant4/G4EmStandardPhysics.hh>
#include <Geant4/G4GammaConversion.hh>
#include <Geant4/G4HadronElasticPhysicsHP.hh>
#include <Geant4/G4HadronPhysicsQGSP_BERT_HP.hh>
#include <Geant4/G4IonElasticPhysics.hh>
#include <Geant4/G4IonPhysics.hh>
#include <Geant4/G4OpticalPhysics.hh>
#include <Geant4/G4ParticleTable.hh>
#include <Geant4/G4PhotoElectricEffect.hh>
#include <Geant4/G4PhysicsListHelper.hh>
#include <Geant4/G4ProcessTable.hh>
#include <Geant4/G4StoppingPhysics.hh>

namespace SiFi {

PhysicsList::PhysicsList() {
    RegisterPhysics(new G4EmStandardPhysics());
    // RegisterPhysics(new G4EmLowEPPhysics());
    RegisterPhysics(new G4EmExtraPhysics());
    RegisterPhysics(new G4OpticalPhysics());
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4StoppingPhysics());
    RegisterPhysics(new G4IonPhysics());
    RegisterPhysics(new G4IonElasticPhysics());
    RegisterPhysics(new G4ChargeExchangePhysics());
    RegisterPhysics(new G4HadronElasticPhysicsHP());
    // RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
}

void PhysicsList::ConstructParticle() {
    log->debug("ConstructParticle()");
    G4VModularPhysicsList::ConstructParticle();
    PhysicsList::DumpList();
    log->debug("ConstructParticle()");
};
} // namespace SiFi
