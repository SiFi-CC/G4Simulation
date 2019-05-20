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

    if (argc != 2) {
        log::info("Usage ./cmd/gen_sim_matrix [OUTPUT]\n"
                  "\n"
                  "OUTPUT - file to save simulation data\n");
        return -1;
    }

    TString output(argv[1]);
    DataStorage storage(output);

    MuraMask mask(
        11, {10. * cm, 10. * cm, 2. * cm}, MaterialManager::get()->GetMaterial("G4_W"));

    auto material = MaterialManager::get()->LuAGCe();
    DetectorBlock detector(
        50,                  // number of layers
        FibreLayer(          //
            100,             // number of fibres in layer
            Fibre({10. * cm, // fibre length
                   0.1 * cm, // fibre width
                   0.1 * cm, // thickness (z-axis)
                   material,
                   material,
                   material})));

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

    const int nIter = 100000;

    //
    // Easiest way to use this part of code is to add break statements
    // at the end of loops that are parametrising values that we don't
    // want to change and override initial value at the begining
    //
    for (int maskDetDistance = 12; maskDetDistance <= 20; maskDetDistance += 2) {
        for (int maskSrcDistance = 10; maskSrcDistance <= 80; maskSrcDistance += 10) {
            maskDetDistance = 16;
            maskSrcDistance = 50;
            construction->setMaskPos(maskSrcDistance * cm);
            construction->setDetectorPos(maskSrcDistance * cm + maskDetDistance * cm);
            runManager.DefineWorldVolume(construction->Construct());
            runManager.GeometryHasBeenModified();

            int maxBinX = 100;
            int maxBinY = 100;
            for (int binX = 0; binX < maxBinX; binX += 1) {
                for (int binY = 0; binY < maxBinY; binY += 1) {
                    // sPosY = sPosX;
                    double sPosX = -10 * cm + (0.5 + binX) * (20 * cm / maxBinX);
                    double sPosY = -10 * cm + (0.5 + binY) * (20 * cm / maxBinY);
                    for (int energy = 4400; energy > 50; energy /= 4) {
                        log::info(
                            "Starting simulation source({}, {}), "
                            "maskToDetectorDistance={}, sourceToMaskDistance={}, "
                            "baseEnergy={}",
                            sPosX,
                            sPosY,
                            maskDetDistance * cm,
                            maskSrcDistance * cm,
                            energy * keV);

                        auto simName = TString::Format(
                            "%f_%f_%d_%d_%d",
                            sPosX,
                            sPosY,
                            maskDetDistance,
                            maskSrcDistance,
                            energy);

                        storage.newSimulation(simName);
                        storage.writeMetadata("sourcePosX", sPosX);
                        storage.writeMetadata("sourcePosY", sPosY);
                        storage.writeMetadata("sourcePosZ", 0 * cm);
                        storage.writeMetadata("energy", energy * keV);
                        storage.writeMetadata(
                            "sourceToMaskDistance", maskSrcDistance * cm);
                        storage.writeMetadata(
                            "maskToDetectorDistance", maskDetDistance * cm);
                        detector.writeMetadata(&storage);
                        mask.writeMetadata(&storage);
                        storage.init();

                        source.GetCurrentSource()->GetPosDist()->SetCentreCoords(
                            G4ThreeVector(sPosX * mm, sPosY * mm, 0));
                        source.GetCurrentSource()->GetEneDist()->SetMonoEnergy(
                            energy * keV);
                        runManager.BeamOn(nIter);
                        storage.cleanup();
                        break;
                    }
                }
            }
            break;
        }
        break;
    }
}
