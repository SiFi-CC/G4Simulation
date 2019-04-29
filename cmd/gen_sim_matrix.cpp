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
        11, {20. * cm, 20. * cm, 3. * cm}, MaterialManager::get()->GetMaterial("G4_W"));

    auto material = MaterialManager::get()->LuAGCe();
    DetectorBlock detector(
        5,                   // number of layers
        FibreLayer(          //
            20,              // number of fibres in layer
            Fibre({20. * cm, // fibre length
                   1. * cm,  // fibre width
                   1. * cm,  // thickness (z-axis)
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

    const int nIter = 200000;

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

            for (int sPosX = -10; sPosX <= 10; sPosX += 1) {
                for (int sPosY = -10; sPosY <= 10; sPosY += 1) {
                    // sPosY = sPosX;
                    for (int energy = 4400; energy > 50; energy /= 4) {
                        log::info(
                            "Starting simulation source({}, {}), "
                            "maskToDetectorDistance={}, sourceToMaskDistance={}, "
                            "baseEnergy={}",
                            sPosX * cm,
                            sPosY * cm,
                            maskDetDistance * cm,
                            maskSrcDistance * cm,
                            energy * keV);

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
                            new TParameter<double>("energy", energy * keV));
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
                        break;
                    }
                }
            }
            break;
        }
        break;
    }
}
