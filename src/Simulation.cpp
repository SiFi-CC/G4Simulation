#include "Simulation.h"
#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "Utils.h"
#include <Geant4/G4RunManager.hh>
#include <Geant4/G4UImanager.hh>

namespace SiFi {

void Simulation::run() {
    log::debug("Simulation::run()");

    log::info("construct the default run manager");
    G4RunManager runManager;

    // set mandatory initialization classes
    runManager.SetUserInitialization(new DetectorConstruction());
    runManager.SetUserInitialization(new PhysicsList());
    runManager.SetUserAction(new PrimaryGeneratorAction("photon", 4.4 * MeV));

    auto eventAction = new EventAction();
    runManager.SetUserAction(eventAction);

    // initialize G4 kernel
    runManager.Initialize();

    // get the pointer to the UI manager and set verbosities
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/run/verbose 1");
    // UI->ApplyCommand("/event/verbose 1");
    // UI->ApplyCommand("/tracking/verbose 1");

    UI->ApplyCommand("/gps/particle gamma");
    UI->ApplyCommand("/gps/pos/type Point");
    UI->ApplyCommand("/gps/pos/center 0 0 0 cm");
    UI->ApplyCommand("/gps/ang/type iso");
    UI->ApplyCommand("/gps/ang/mintheta 165 deg");
    UI->ApplyCommand("/gps/ene/type Mono");
    UI->ApplyCommand("/gps/energy 4.4 MeV");

    // start a run
    int numberOfEvent = 100000;
    runManager.BeamOn(numberOfEvent);
}

} // namespace SiFi
