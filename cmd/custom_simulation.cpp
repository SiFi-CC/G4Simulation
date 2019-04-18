#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "Simulation.h"
#include "tracking/SteppingAction.h"
#include <TParameter.h>

using namespace SiFi;

int main(int argc, char** argv) {
    log::set_level(spdlog::level::info);
    DataStorage storage("./simulation_results.root");

    int maskDetDistance = 14;
    int maskSrcDistance = 40;
    int sPosX = 0;
    int sPosY = 0;
    int energy = 4400;

    MuraMask mask(
        11, {20. * cm, 20. * cm, 3. * cm}, MaterialManager::get()->GetMaterial("G4_W"));
    DetectorBlock detector(defaults::geometry::simpleDetectorBlock());
    auto construction = new DetectorConstruction(&mask, &detector);
    construction->setMaskPos(maskSrcDistance * cm);
    construction->setDetectorPos(maskSrcDistance * cm + maskDetDistance * cm);

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
        G4ThreeVector(sPosX * cm, sPosY * cm, 0));
    source.GetCurrentSource()->GetEneDist()->SetMonoEnergy(energy * keV);

    runManager.SetUserAction(new PrimaryGeneratorAction(&source));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    const int nIter = 2000000;

    log::info(
        "Starting simulation source({}, {}), "
        "maskToDetectorDistance={}, sourceToMaskDistance={}, "
        "baseEnergy={}",
        sPosX * cm,
        sPosY * cm,
        maskDetDistance * cm,
        maskSrcDistance * cm,
        energy * keV);

    storage.newSimulation(
        TString::Format(
            "%d_%d_%d_%d_%d", sPosX, sPosY, maskDetDistance, maskSrcDistance, energy),
        true);

    storage.writeMetadata(new TParameter<double>("sourcePosX", sPosX * cm));
    storage.writeMetadata(new TParameter<double>("sourcePosY", sPosY * cm));
    storage.writeMetadata(new TParameter<double>("sourcePosZ", 0 * cm));
    storage.writeMetadata(new TParameter<double>("energy", energy * keV));
    storage.writeMetadata(
        new TParameter<double>("sourceToMaskDistance", maskSrcDistance * cm));
    storage.writeMetadata(
        new TParameter<double>("maskToDetectorDistance", maskDetDistance * cm));

    runManager.BeamOn(nIter);
    storage.cleanup();
}
