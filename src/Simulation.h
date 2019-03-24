#pragma once

#include "DataStorage.h"
#include "DetectorConstruction.h"
#include "Utils.h"
#include <Geant4/G4RunManager.hh>

namespace SiFi {

enum SimulationStatus { empty, prepared, runned, errored };

class Simulation {
  public:
    Simulation(int argc, char** argv) {
        fProgramCmd.argc = argc;
        fProgramCmd.argv = argv;
    };
    void prepareDefault();
    void prepare(DetectorElement* mask, DetectorElement* absorber, double distance);
    void prepare(G4VUserDetectorConstruction* construction);

    void print();

    void showGeometry();
    void run(int nEvents);
    void runScript(const G4String& macrofile);

    void setDataStorage(DataStorage* storage) { fDataStorage = storage; }

    const logger log = createLogger("Simulation");

  private:
    void doPrepare(G4VUserDetectorConstruction* construction);
    struct {
        int argc;
        char** argv;
    } fProgramCmd = {0, nullptr};

    SimulationStatus fStatus = empty;
    G4RunManager fRunManager;
    DataStorage* fDataStorage = nullptr;
};

} // namespace SiFi
