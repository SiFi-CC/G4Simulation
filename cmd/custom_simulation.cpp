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

    CmdLineOption opt_det(
      "Plane", "-det",
      "Detector: detector-source:nFibres:fibre_width, default: 200:1:22:22", 0, 0);
    CmdLineOption opt_mask(
      "Mask", "-mask",
      "Mask: order:mask-source:width:length:thickness [mm], default: 31:150:22:20", 0, 0);
    CmdLineOption opt_events("Events", "-n",
                               "Number of events, default: 1000 (integer)", 1000);
    CmdLineOption opt_energy("Energy", "-e",
                               "Energy of particles [keV], default: 4400 (integer)", 4400);
    CmdLineOption opt_theta("Theta", "-theta",
                               "Min and max Theta [Deg], default: 170:180", 0);
    CmdLineOption opt_source("Source", "-source",
                               "Source position [mm], default: 0:0", 0);
    CmdLineOption opt_visualization("Visualization", "-vis",
                               "Run visualization");
    CmdLineArg cmdarg_output("output", "Output file", CmdLineArg::kString);

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

    const Positional& args = CmdLineConfig::GetPositionalArguments();

    TString output(args.at("output")->GetStringValue());
    DataStorage storage(output);

    Float_t detectorsource = 200, fibrewidth = 1.3; 
    Int_t fibrenum = 22;

    Int_t mord = 31;
    Float_t masksource = 150., masklength = 64., maskthick = 20.;

    Float_t minTheta, maxTheta = 180;

    Float_t sPosX = 0, sPosY = 0;
    { //CmdLine options 
        if (opt_det.GetArraySize() == 3) {
            detectorsource = opt_det.GetDoubleArrayValue(1);
            fibrenum = opt_det.GetIntArrayValue(2);
            fibrewidth = opt_det.GetDoubleArrayValue(3);
        } else if (opt_det.GetArraySize() != 0) {
            spdlog::error("Detector plane - 3 parameters required, {} given",
                          opt_det.GetArraySize());
            abort();
        }
        if (opt_mask.GetArraySize() == 4) {
            mord = opt_mask.GetIntArrayValue(1);
            masksource = opt_mask.GetDoubleArrayValue(2);
            masklength = opt_mask.GetDoubleArrayValue(3);
            maskthick = opt_mask.GetDoubleArrayValue(4);
        } else if (opt_mask.GetArraySize() != 0) {
            spdlog::error("Mask plane - 4 parameters required, {} given",
                          opt_mask.GetArraySize());
            abort();
        }

        // minTheta = atan(-(masklength+fibrewidth*fibrenum)*sqrt(2)/2/detectorsource)*180/M_PI+180.;
        minTheta = atan(-(masklength+fibrewidth*fibrenum)/2/detectorsource)*180/M_PI+180.;
        if (opt_theta.GetArraySize() == 2) {
            minTheta = opt_theta.GetDoubleArrayValue(1);
            maxTheta = opt_theta.GetDoubleArrayValue(2);
        } else if (opt_theta.GetArraySize() != 0) {
            spdlog::error("Theta angles - 2 parameters required: min and max values, {} given",
                          opt_theta.GetArraySize());
            abort();
        }
        if (opt_source.GetArraySize() == 2) {
            sPosX = opt_source.GetDoubleArrayValue(1);
            sPosY = opt_source.GetDoubleArrayValue(2);
        } else if (opt_source.GetArraySize() != 0) {
            spdlog::error("Source position - 2 parameters required, {} given",
                          opt_source.GetArraySize());
            abort();
        }
    }//CmdLine options 



    printf("Detector : %g %g %i %g [mm]\n", detectorsource, fibrenum*fibrewidth, fibrenum, fibrewidth);
    printf("Mask     : %g %g %g %g [mm]\n", masksource, masklength, masklength, maskthick);
    printf("Mask order      : %i\n", mord);
    printf("No. of events  : %i\n", opt_events.GetIntValue());
    printf("Energy [keV] : %i\n", opt_energy.GetIntValue());
    printf("Theta [Deg] : %g %g\n", minTheta, maxTheta);
    printf("Source position [mm] : %g %g\n", sPosX, sPosY);

    //choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    //set random seed with system time
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed(seed);

    double maskdetector = (detectorsource-masksource);
    int energy = opt_energy.GetIntValue();
    // auto material = MaterialManager::get()->LuAGCe();
    auto material = MaterialManager::get()->LYSO();
    auto wrappingmaterial = MaterialManager::get()->GetMaterial("G4_Al");
    auto airmaterial = MaterialManager::get()->GetMaterial("G4_AIR");

    MuraMask mask(
        mord, {masklength * mm, masklength * mm, maskthick * mm}, MaterialManager::get()->GetMaterial("G4_W"));
    int nLayer = 10;
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

    runManager.SetUserAction(new PrimaryGeneratorAction(source.GetSource()));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

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

    
    if (CmdLineOption::GetFlagValue("Visualization")){
        //VISUALISATION:
        G4VisManager* visManager = new G4VisExecutive;
            visManager->Initialize();

        G4UImanager* UI = G4UImanager::GetUIpointer();

        G4UIExecutive* UI2 = new G4UIExecutive(argc, argv);
                    UI->ApplyCommand("/control/execute vis_CC.mac");
                    UI2->SessionStart();
                    delete UI2;
        delete visManager;
    } else {
        runManager.BeamOn(nIter);
    }

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
}
