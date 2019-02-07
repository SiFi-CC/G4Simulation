#include "Simulation.h"
#include "ActionInitialization.h"
#include "DetectorConstruction.h"
#include "PhysicsList.h"
#include <Geant4/G4RunManager.hh>
#include <Geant4/G4UImanager.hh>

namespace SiFi {

void Simulation::run() {
    // construct the default run manager
    G4RunManager runManager;

    // set mandatory initialization classes
    runManager.SetUserInitialization(new DetectorConstruction());
    runManager.SetUserInitialization(new PhysicsList());
    runManager.SetUserInitialization(new ActionInitialization());

    // initialize G4 kernel
    runManager.Initialize();

    // get the pointer to the UI manager and set verbosities
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/run/verbose 1");
    UI->ApplyCommand("/event/verbose 1");
    UI->ApplyCommand("/tracking/verbose 1");

    // start a run
    int numberOfEvent = 3;
    runManager.BeamOn(numberOfEvent);
}

} // namespace SiFi
