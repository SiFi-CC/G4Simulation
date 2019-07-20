#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"
#include <Geant4/G4RunManager.hh>
#include <TParameter.h>

using namespace SiFi;

int main(int argc, char** argv) {
    log::set_level(spdlog::level::info);
    DataStorage storage("./simulation_results.root");

    MuraMask mask(11, {20. * cm, 20. * cm, 3. * cm}, MaterialManager::get()->LuAGCe());
    DetectorBlock detector(defaults::geometry::simpleDetectorBlock());
    auto construction = new DetectorConstruction(&mask, &detector);
    construction->setMaskPos(41 * cm);
    construction->setDetectorPos(57.5 * cm);

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager.SetUserInitialization(physicsList);

    G4GeneralParticleSource source;
    source.GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    source.GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
    source.GetCurrentSource()->GetAngDist()->SetMinTheta(120 * deg);
    source.GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    source.GetCurrentSource()->GetPosDist()->SetCentreCoords(
        G4ThreeVector(10 * cm, 10 * cm, 0));
    source.GetCurrentSource()->GetEneDist()->SetMonoEnergy(4.4 * MeV);

    runManager.SetUserAction(new PrimaryGeneratorAction(&source));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    const int nIter = 200000;

    storage.newSimulation("test_sim", true);
    runManager.BeamOn(nIter);
    storage.cleanup();
}
