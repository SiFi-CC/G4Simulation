#include "PhysicsList.h"
#include "Utils.h"
#include <Geant4/G4ComptonScattering.hh>
#include <Geant4/G4Electron.hh>
#include <Geant4/G4Gamma.hh>
#include <Geant4/G4GammaConversion.hh>
#include <Geant4/G4ParticleTable.hh>
#include <Geant4/G4PhotoElectricEffect.hh>
#include <Geant4/G4PhysicsListHelper.hh>
#include <Geant4/G4Positron.hh>
#include <Geant4/G4ProcessTable.hh>
#include <Geant4/G4Proton.hh>
#include <Geant4/G4RayleighScattering.hh>

namespace SiFi {

void PhysicsList::ConstructParticle() {
    log->debug("ConstructParticle()");
    G4Proton::ProtonDefinition();
    G4Gamma::GammaDefinition();
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    PhysicsList::DumpList();
    log->debug("ConstructParticle()");
};
void PhysicsList::ConstructProcess() {
    log->debug("ConstructProcess()");
    AddTransportation();

    auto ph = G4PhysicsListHelper::GetPhysicsListHelper();
    auto particle = G4Gamma::GammaDefinition();
    ph->RegisterProcess(new G4PhotoElectricEffect(), particle);
    ph->RegisterProcess(new G4ComptonScattering(), particle);
    ph->RegisterProcess(new G4GammaConversion(), particle);
    ph->RegisterProcess(new G4RayleighScattering(), particle);

    auto processNameList = G4ProcessTable::GetProcessTable()->GetNameList();
    for (G4int i = 0; i < G4int(processNameList->size()); i++) {
        log->debug("AddProcess: {}", (*processNameList)[i]);
    }
    log->debug("ConstructProcess()");
};
} // namespace SiFi
