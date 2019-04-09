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

    MuraMask mask(11, {20. * cm, 20. * cm, 6. * cm}, MaterialManager::get()->LuAGCe());
    DetectorBlock detector(defaults::geometry::simpleDetectorBlock());
    auto construction = new DetectorConstruction(&mask, &detector);

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager.SetUserInitialization(physicsList);

    G4GeneralParticleSource source;
    source.GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    source.GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
    source.GetCurrentSource()->GetAngDist()->SetMinTheta(120 * deg);
    source.GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");

    runManager.SetUserAction(new PrimaryGeneratorAction(&source));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    const int nIter = 200000;

    for (int maskDetDistance = 12; maskDetDistance <= 20; maskDetDistance += 2) {
        for (int maskSrcDistance = 10; maskSrcDistance <= 80; maskSrcDistance += 10) {
            construction->setMaskPos(maskSrcDistance * cm);
            construction->setDetectorPos(maskSrcDistance * cm + maskDetDistance * cm);
            runManager.DefineWorldVolume(construction->Construct());
            runManager.GeometryHasBeenModified();

            for (int sPosX = -10; sPosX <= 10; sPosX += 10) {
                for (int sPosY = -10; sPosY <= 10; sPosY += 10) {
                    sPosY = sPosX;
                    for (int energy = 4400; energy > 50; energy /= 4) {
                        log::info(
                            "Starting simulation source({}, {}), "
                            "maskToDetectorDistance={}, sourceToMaskDistance={}, "
                            "baseEnergy={}",
                            sPosX * cm,
                            sPosY * cm,
                            maskDetDistance * cm,
                            maskSrcDistance * cm,
                            energy * MeV / 1000);

                        storage.newSimulation(TString::Format(
                            "%d_%d_%d_%d_%d",
                            sPosX,
                            sPosY,
                            maskDetDistance,
                            maskSrcDistance,
                            energy));

                        storage.writeMetadata(
                            new TParameter<double>("sourcePosX", sPosX * cm));
                        storage.writeMetadata(
                            new TParameter<double>("sourcePosY", sPosY * cm));
                        storage.writeMetadata(
                            new TParameter<double>("sourcePosZ", 0 * cm));
                        storage.writeMetadata(
                            new TParameter<double>("energy", energy * MeV / 1000));
                        storage.writeMetadata(new TParameter<double>(
                            "sourceToMaskDistance", maskSrcDistance * cm));
                        storage.writeMetadata(new TParameter<double>(
                            "maskToDetectorDistance", maskDetDistance * cm));

                        source.GetCurrentSource()->GetPosDist()->SetCentreCoords(
                            G4ThreeVector(sPosX * cm, sPosY * cm, 0));
                        source.GetCurrentSource()->GetEneDist()->SetMonoEnergy(
                            energy * keV);
                        runManager.BeamOn(nIter);
                        storage.cleanup();
                    }
                    break;
                }
            }
        }
    }
}
