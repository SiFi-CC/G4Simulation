#include <TParameter.h>

#include <CmdLineConfig.hh>
#include <G4RunManager.hh>

#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"
#include "Source.hh"

using namespace SiFi;

int main(int argc, char** argv) {
     CmdLineOption opt_det(
      "Plane", "-det",
      "Detector: detector-source:fibre_length[mm]:nFibres:fibre_width[mm], default: 175.4:22:22:1", 0, 0);
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
                               "Source plane size and number of bins [mm], default: 22:22:100:100", 0);

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

    Float_t xDimSource = 2.2 * cm, yDimSource = 2.2 * cm;
    Int_t maxBinX = 100, maxBinY = 100;

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
    if (opt_source.GetArraySize() == 4) {
        xDimSource = opt_source.GetDoubleArrayValue(1)/10. * cm;
        yDimSource = opt_source.GetDoubleArrayValue(2)/10. * cm;
        maxBinX = opt_source.GetIntArrayValue(3);
        maxBinY = opt_source.GetIntArrayValue(4);
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
    printf("Source plane [mm] : %g %g Number of bins: %i %i\n", xDimSource, yDimSource, maxBinX, maxBinY);

    double maskDetDistance = (ds-ms)/10;
    double maskSrcDistance = ms/10;
    int energy = opt_energy.GetIntValue();
    auto material = MaterialManager::get()->LuAGCe();

    MuraMask mask(
        mord, {mw/10 * cm, ml/10. * cm, mt/10. * cm}, MaterialManager::get()->GetMaterial("G4_W"));
    DetectorBlock detector(
        50,                 // number of layers
        FibreLayer(          //
            dn,             // number of fibres in layer
            Fibre({dl/10. * cm, // fibre length
                   dw/10. * cm, // fibre width
                   0.1 * cm, // thickness (z-axis)
                   material,
                   material,
                   material})));

    auto construction = new DetectorConstruction(&mask, &detector);

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager.SetUserInitialization(physicsList);

    Source source(energy,minTheta,maxTheta);
    
    runManager.SetUserAction(new PrimaryGeneratorAction(source.GetSource()));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    const int nIter = opt_events.GetIntValue();

    storage.setBinnedSize(maxBinX, maxBinY, dn, dn);
    storage.resizeHmatrix();

    storage.newSimulation(
            TString::Format(
                "%d_%d_%d", maskDetDistance, maskSrcDistance, energy),false);
    
    storage.writeMetadata("energy", energy * keV);
    storage.writeMetadata(
        "sourceToMaskDistance", maskSrcDistance * cm);
    storage.writeMetadata(
        "maskToDetectorDistance", maskDetDistance * cm);
    detector.writeMetadata(&storage);
    mask.writeMetadata(&storage);
    storage.writeMetadata("sourceMinX", - xDimSource / 2);
    storage.writeMetadata("sourceMaxX", xDimSource / 2);
    storage.writeMetadata("sourceMinY", - yDimSource / 2);
    storage.writeMetadata("sourceMaxY", yDimSource / 2);
    storage.writeMetadata("sourceBinX", maxBinX);
    storage.writeMetadata("sourceBinY", maxBinY);
    storage.init();

    //
    // Easiest way to use this part of code is to add break statements
    // at the end of loops that are parametrising values that we don't
    // want to change and override initial value at the begining
    //
    log::info("maskDetDistance {}, maskSrcDistance {}",maskDetDistance * cm,maskSrcDistance * cm);
    construction->setMaskPos(maskSrcDistance * cm);
    construction->setDetectorPos(maskSrcDistance * cm + maskDetDistance * cm);
    runManager.DefineWorldVolume(construction->Construct());
    runManager.GeometryHasBeenModified();


    for (int binX = 0; binX < maxBinX; binX += 1) {
        for (int binY = 0; binY < maxBinY; binY += 1) {
            double sPosX = - xDimSource / 2. + (0.5 + binX) * (xDimSource / maxBinX);
            double sPosY = - yDimSource / 2. + (0.5 + binY) * (yDimSource / maxBinY);
            for (int energy_it = energy; energy_it > 50; energy_it /= 4) {
                log::info(
                    "Starting simulation source({}, {}), "
                    "maskToDetectorDistance={}, sourceToMaskDistance={}, "
                    "baseEnergy={}",
                    sPosX,
                    sPosY,
                    maskDetDistance * cm,
                    maskSrcDistance * cm,
                    energy_it * keV);
                storage.setCurrentBins(binX, binY);

                source.SetPos(TVector3(sPosX, sPosY, 0));
                runManager.BeamOn(nIter);
                break;
            }
        }
    }
    storage.writeHmatrix("Hmatrix.root");
    storage.cleanup();
}
