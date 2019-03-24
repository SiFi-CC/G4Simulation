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

    MuraMask mask(defaults::geometry::simpleMask());
    DetectorBlock detector(defaults::geometry::simpleDetectorBlock());
    auto construction = new DetectorConstruction(&mask, &detector);

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    runManager.SetUserInitialization(new PhysicsList());

    G4GeneralParticleSource source;
    source.GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    source.GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
    source.GetCurrentSource()->GetAngDist()->SetMinTheta(165 * deg);
    source.GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");

    runManager.SetUserAction(new PrimaryGeneratorAction(&source));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    const int nIter = 100000;

    for (int maskDetDistance = 20; maskDetDistance < 30; maskDetDistance += 5) {
        for (int maskSrcDistance = 5; maskSrcDistance < 60; maskSrcDistance += 5) {
            construction->setMaskPos(maskSrcDistance * cm + mask.getThickness() / 2);
            construction->setDetectorPos(
                maskSrcDistance * cm + maskDetDistance * cm +
                detector.getThickness() / 2);
            runManager.DefineWorldVolume(construction->Construct());
            runManager.GeometryHasBeenModified();

            for (int sPosX = 0; sPosX <= 10; sPosX++) {
                for (int sPosY = 0; sPosY <= 10; sPosY++) {
                    for (int energy = 4400; energy > 100; energy /= 2) {
                        log::info(
                            "Starting simulation source({}, {}), maskToDetDistance={}, "
                            "sourceToMaskDistance={}, baseEenrgy={}",
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
                            new TParameter<double>("energy", energy * MeV / 1000));
                        storage.writeMetadata(new TParameter<double>(
                            "srcMaskDistance", maskSrcDistance * cm));
                        storage.writeMetadata(new TParameter<double>(
                            "maskDetDistance", maskDetDistance * cm));

                        source.GetCurrentSource()->GetPosDist()->SetCentreCoords(
                            G4ThreeVector(sPosX * cm, sPosY * cm, 0));
                        source.GetCurrentSource()->GetEneDist()->SetMonoEnergy(
                            energy * keV);
                        runManager.BeamOn(nIter);
                        storage.cleanup();
                        break;
                    }
                    break;
                }
                break;
            }
        }
        break;
    }
}
