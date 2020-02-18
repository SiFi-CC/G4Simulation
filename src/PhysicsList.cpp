#include "PhysicsList.h"
#include "Utils.h"
#include <G4ChargeExchangePhysics.hh>
#include <G4ComptonScattering.hh>
#include <G4DecayPhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmLowEPPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4GammaConversion.hh>
#include <G4HadronElasticPhysicsHP.hh>
#include <G4HadronPhysicsQGSP_BERT_HP.hh>
#include <G4IonElasticPhysics.hh>
#include <G4IonPhysics.hh>
#include <G4OpticalPhysics.hh>
#include <G4ParticleTable.hh>
#include <G4PhotoElectricEffect.hh>
#include <G4PhysicsListHelper.hh>
#include <G4ProcessTable.hh>
#include <G4StoppingPhysics.hh>

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
}
} // namespace SiFi
