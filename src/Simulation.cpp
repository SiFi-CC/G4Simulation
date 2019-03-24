

#include "Simulation.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "Utils.h"
#include "tracking/SteppingAction.h"
#include <Geant4/G4UIExecutive.hh>
#include <Geant4/G4UImanager.hh>
#include <Geant4/G4UItcsh.hh>
#include <Geant4/G4UIterminal.hh>
#include <Geant4/G4VisExecutive.hh>
#include <Geant4/G4VisManager.hh>

namespace SiFi {

void Simulation::prepareDefault() {
    log->info("Setting up default simulation");
    doPrepare(new DetectorConstruction(
        new MuraMask(defaults::geometry::simpleMask()),
        new DetectorBlock(defaults::geometry::simpleDetectorBlock())));
}

void Simulation::prepare(DetectorElement* mask, DetectorElement* absorber) {
    log->info("Setting up standart cm simulation");
    doPrepare(new DetectorConstruction(mask, absorber));
}

void Simulation::prepare(G4VUserDetectorConstruction* construction) {
    log->info("Setting up simulation with custom geometry");
    doPrepare(construction);
}

void Simulation::doPrepare(G4VUserDetectorConstruction* construction) {
    fRunManager.SetUserInitialization(construction);
    fRunManager.SetUserInitialization(new PhysicsList());
}

void Simulation::run(int numberOfEvent) {
    log::debug("Simulation::run()");
    fRunManager.SetUserAction(new PrimaryGeneratorAction(new G4GeneralParticleSource()));
    fRunManager.SetUserAction(new SteppingAction(fDataStorage));
    fRunManager.SetUserAction(new EventAction(fDataStorage));
    fRunManager.Initialize();

    fRunManager.BeamOn(numberOfEvent);
}

void Simulation::runScript(const G4String& macro) {
    fRunManager.Initialize();
    log::debug("Simulation::runScript({})", macro);
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/control/execute " + macro);

    int numberOfEvent = 0;
    fRunManager.BeamOn(numberOfEvent);
}

void Simulation::showGeometry() {
    fRunManager.Initialize();
    log::info("G4VisManager");
    G4VisExecutive visManager;
    visManager.Initialize();

    log::info("GetUIpointer()");
    G4UImanager* UI = G4UImanager::GetUIpointer();

    log::info("G4UIExecutive()");
    {
        G4UIExecutive UI2 = G4UIExecutive(fProgramCmd.argc, fProgramCmd.argv);
        UI->ApplyCommand("/control/execute ../macros/geometryDebug.mac");

        UI2.SessionStart();
    }
}

} // namespace SiFi
