#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"

#include <TParameter.h>
#include <TFile.h>
#include <TString.h>

#include <G4RunManager.hh>
#include <CmdLineConfig.hh>

#include "Source.hh"

using namespace SiFi;

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);

    CmdLineOption opt_det(
      "Plane", "-det",
      "Detector: detector-source:fibre_length:nFibres:fibre_width, default: 175.4:1:22:22", 0, 0);
    CmdLineOption opt_mask(
      "Mask", "-mask",
      "Mask: order:mask-source:width:length:thickness [mm], default: 37:100:22:22:20", 0, 0);
    CmdLineOption opt_events("Events", "-n",
                               "Number of events, default: 1000 (integer)", 1000);
    CmdLineOption opt_energy("Energy", "-e",
                               "Energy of particles [keV], default: 4400 (integer)", 4400);
    CmdLineOption opt_theta("Theta", "-theta",
                               "Min and max Theta [Deg], default: 170:180", 0);
    CmdLineOption opt_source("Source", "-source",
                               "Source position [mm], default: -2:2", 0);
    CmdLineArg cmdarg_output("output", "Output file", CmdLineArg::kString);

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

    const Positional& args = CmdLineConfig::GetPositionalArguments();

    TString output(args.at("output")->GetStringValue());
    DataStorage storage(output);

    Float_t ds = 175.4, dw = 1., dl = 22.; 
    Int_t dn = 22;

    Int_t mord = 37;
    Float_t ms = 100., mw = 22., ml = 22., mt = 20.;

    Float_t minTheta = 170., maxTheta = 180;

    Float_t sPosX = -0.2, sPosY = 0.2;

    if (opt_det.GetArraySize() == 4) {
        ds = opt_det.GetDoubleArrayValue(1);
        dl = opt_det.GetDoubleArrayValue(2);
        dn = opt_det.GetIntArrayValue(3);
        dw = opt_det.GetDoubleArrayValue(4);
    } else if (opt_det.GetArraySize() != 0) {
        spdlog::error("Detector plane - 4 parameters required, {} given",
                      opt_det.GetArraySize());
        abort();
    }
    if (opt_mask.GetArraySize() == 5) {
        mord = opt_mask.GetIntArrayValue(1);
        ms = opt_mask.GetDoubleArrayValue(2);
        mw = opt_mask.GetDoubleArrayValue(3);
        ml = opt_mask.GetDoubleArrayValue(4);
        mt = opt_mask.GetDoubleArrayValue(5);
    } else if (opt_mask.GetArraySize() != 0) {
        spdlog::error("Mask plane - 5 parameters required, {} given",
                      opt_mask.GetArraySize());
        abort();
    }

    if (opt_theta.GetArraySize() == 2) {
        minTheta = opt_theta.GetDoubleArrayValue(1);
        maxTheta = opt_theta.GetDoubleArrayValue(2);
    } else if (opt_theta.GetArraySize() != 0) {
        spdlog::error("Theta angles - 2 parameters required: min and max values, {} given",
                      opt_theta.GetArraySize());
        abort();
    }
    if (opt_source.GetArraySize() == 2) {
        sPosX = opt_source.GetDoubleArrayValue(1)/10.;
        sPosY = opt_source.GetDoubleArrayValue(2)/10.;
    } else if (opt_source.GetArraySize() != 0) {
        spdlog::error("Source position - 2 parameters required, {} given",
                      opt_source.GetArraySize());
        abort();
    }

    printf("Detector : %g %g %i %g [mm]\n", ds, dl, dn, dw);
    printf("Mask     : %g %g %g %g [mm]\n", ms, mw, ml, mt);
    printf("Mask order      : %i\n", mord);
    printf("No. of events  : %i\n", opt_events.GetIntValue());
    printf("Energy [keV] : %i\n", opt_energy.GetIntValue());
    printf("Theta [Deg] : %g %g\n", minTheta, maxTheta);
    printf("Source position [mm] : %g %g\n", sPosX*10., sPosY*10.);

    double maskDetDistance = (ds-ms)/10;
    double maskSrcDistance = ms/10;
    int energy = opt_energy.GetIntValue();
    auto material = MaterialManager::get()->LuAGCe();

    MuraMask mask(
        mord, {mw/10 * cm, ml/10. * cm, mt/10. * cm}, MaterialManager::get()->GetMaterial("G4_W"));
    DetectorBlock detector(
        50,                  // number of layers
        FibreLayer(          //
            dn,             // number of fibres in layer
            Fibre({dl/10. * cm, // fibre length
                   dw/10. * cm, // fibre width
                   0.1 * cm, // thickness (z-axis)
                   material,
                   material,
                   material})));

    auto construction = new DetectorConstruction(&mask, &detector);
    construction->setMaskPos(maskSrcDistance * cm);
    construction->setDetectorPos(maskSrcDistance * cm + maskDetDistance * cm);

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
        sPosX * cm,
        sPosY * cm,
        maskDetDistance * cm,
        maskSrcDistance * cm,
        energy * keV);

    storage.newSimulation(
        TString::Format(
            "%d_%d_%d_%d_%d", sPosX, sPosY, maskDetDistance, maskSrcDistance, energy),
        true);

    storage.writeMetadata("sourcePosX", sPosX * cm);
    storage.writeMetadata("sourcePosY", sPosY * cm);
    storage.writeMetadata("sourcePosZ", 0 * cm);
    storage.writeMetadata("energy", energy * keV);
    storage.writeMetadata("sourceToMaskDistance", maskSrcDistance * cm);
    storage.writeMetadata("maskToDetectorDistance", maskDetDistance * cm);
    detector.writeMetadata(&storage);
    mask.writeMetadata(&storage);
    storage.init(); 

    runManager.BeamOn(nIter);
    storage.cleanup();
}
