#include <mpi.h>

#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"
#include "Source.hh"

#include <TParameter.h>
#include <TFile.h>
#include <TString.h>

#include <G4RunManager.hh>
#include <G4VisManager.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include "Randomize.hh"
#include "time.h"

#include <CmdLineConfig.hh>

#include <math.h>

using namespace SiFi;

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);

    CmdLineOption opt_events("Events", "-n",
                           "Number of events, default: 1000 (integer)", 1000);

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    spdlog::info("processor {}",world_rank);

        //choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    //set random seed with system time
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed((world_rank+1)*seed);


    TString output("mpi"+std::to_string(world_rank)+".root");
    // TString output("mpi.root");
    DataStorage storage(output);

    Float_t detectorsource = 200, fibrewidth = 1.3; 
    Int_t fibrenum = 22;

    Int_t mord = 31;
    Float_t masksource = 150., masklength = 64., maskthick = 20.;

    Float_t minTheta=178, maxTheta = 180;

    Float_t sPosX = 0, sPosY = 0;


    printf("Detector : %g %g %i %g [mm]\n", detectorsource, fibrenum*fibrewidth, fibrenum, fibrewidth);
    printf("Mask     : %g %g %g %g [mm]\n", masksource, masklength, masklength, maskthick);
    printf("Mask order      : %i\n", mord);
    printf("Theta [Deg] : %g %g\n", minTheta, maxTheta);
    printf("Source position [mm] : %g %g\n", sPosX, sPosY);

    double maskdetector = (detectorsource-masksource);
    int energy = 4400;
    // auto material = MaterialManager::get()->LuAGCe();
    auto material = MaterialManager::get()->LYSO();
    auto wrappingmaterial = MaterialManager::get()->GetMaterial("G4_Al");
    auto airmaterial = MaterialManager::get()->GetMaterial("G4_AIR");

    MuraMask mask(
        mord, {masklength * mm, masklength * mm, maskthick * mm}, MaterialManager::get()->GetMaterial("G4_W"));
        // mord, {masklength * mm, masklength * mm, maskthick * mm}, airmaterial);
    int nLayer = 50;
    DetectorBlock detector(
        nLayer,                 // number of layers
        FibreLayer(          //
            fibrenum,             // number of fibres in layer
            Fibre({fibrewidth*fibrenum *mm, // fibre length
                   fibrewidth *mm, // fibre width and thickness
                   material,
                   wrappingmaterial,
                   airmaterial})));

    auto construction = new DetectorConstruction(&mask, &detector);
    construction->setMaskPos(masksource * mm);
    construction->setDetectorPos(detectorsource * mm + nLayer*fibrewidth/2 *mm);

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager.SetUserInitialization(physicsList);
    
    Source source(energy,minTheta,maxTheta);

    source.SetPos(TVector3(sPosX, sPosY, 0));
    // spdlog::info("seed = {}, Random = {}",seed,G4UniformRand());
    // exit(0);

    runManager.SetUserAction(new PrimaryGeneratorAction(source.GetSource()));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    // const int nIter = 1000;
    const int nIter = opt_events.GetIntValue();

    log::info(
        "Starting simulation source({}, {}), "
        "maskToDetectorDistance={}, sourceToMaskDistance={}, "
        "baseEnergy={}",
        sPosX * mm,
        sPosY * mm,
        maskdetector * mm,
        masksource * mm,
        energy * keV);

    storage.newSimulation(
        TString::Format(
            "%g_%g_%g_%g_%d", sPosX, sPosY, maskdetector, masksource, energy),
        true);

    storage.writeMetadata("sourcePosX", sPosX * mm);
    storage.writeMetadata("sourcePosY", sPosY * mm);
    storage.writeMetadata("sourcePosZ", 0 * mm);
    storage.writeMetadata("energy", energy * keV);
    storage.writeMetadata("sourceToMaskDistance", masksource * mm);
    storage.writeMetadata("maskToDetectorDistance", maskdetector * mm);
    detector.writeMetadata(&storage);
    mask.writeMetadata(&storage);
    storage.init(); 


    // if (CmdLineOption::GetFlagValue("Visualization")){
    //     //VISUALISATION:
    //     G4VisManager* visManager = new G4VisExecutive;
    //         visManager->Initialize();

    //     G4UImanager* UI = G4UImanager::GetUIpointer();

    //     G4UIExecutive* UI2 = new G4UIExecutive(argc, argv);
    //                 UI->ApplyCommand("/control/execute vis_CC.mac");
    //                 UI2->SessionStart();
    //                 delete UI2;
    //     delete visManager;
    // } else {
        runManager.BeamOn(nIter);
    // }

    //MULTIPLE POINTS:
    // printf("Iter");

    // source.SetPos(TVector3(-2, -2, 0));
    // runManager.GeometryHasBeenModified();
    // runManager.BeamOn(nIter);

    // printf("Iter");

    //  source.SetPos(TVector3(0, 1, 0));
    // runManager.GeometryHasBeenModified();
    // runManager.BeamOn(nIter);

    storage.cleanup();
     MPI_Finalize();
}
