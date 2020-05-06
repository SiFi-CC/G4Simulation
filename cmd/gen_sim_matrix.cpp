#include <TParameter.h>

#include <CmdLineConfig.hh>
#include <G4RunManager.hh>

#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"
#include "Source.hh"

#include <math.h>

using namespace SiFi;

int main(int argc, char** argv) {
     CmdLineOption opt_det(
      "Plane", "-det",
      "Detector: detector-source:nFibres:fibre_width[mm], default: 200:22:1.3", 0, 0);
    CmdLineOption opt_mask(
      "Mask", "-mask",
      "Mask: order:mask-source:width:length:thickness [mm], default: 31:150:64:20", 0, 0);
    CmdLineOption opt_masktype(
      "MaskType", "-masktype", "MaskType: standart, round or pet", "standart");
    CmdLineOption opt_events("Events", "-n",
                               "Number of events, default: 1000 (integer)", 1000);
    CmdLineOption opt_energy("Energy", "-e",
                               "Energy of particles [keV], default: 4400 (integer)", 4400);
    CmdLineOption opt_nlay("#layers", "-nlay",
                               "Number of layers in detector, default: 4 (integer)", 4);

    CmdLineOption opt_theta("Theta", "-theta",
                               "Min and max Theta [Deg], default: 170:180", 0);
    CmdLineOption opt_source("Source", "-source",
                               "Source plane size and number of bins [mm], default: 64:100", 0);

    CmdLineArg cmdarg_output("output", "Output file", CmdLineArg::kString);

    CmdLineOption opt_precision("Precision", "-er",
                               "Error in W rods size, default: 0.0[mm]"
                               "(relevant only for pet and nowallpet masks", 0.0);

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

    const Positional& args = CmdLineConfig::GetPositionalArguments();

    TString output(args.at("output")->GetStringValue());
    DataStorage storage(output);

    Float_t detectorsource = 200, fibrewidth = 1.3; 
    Int_t fibrenum = 22;

    Int_t mord = 31;
    Float_t masksource = 150., masklength = 64., maskthick = 20.;

    Float_t minTheta = 170., maxTheta = 180;

    Float_t xDimSource = 64, yDimSource = 64;
    Int_t maxBinX = 100, maxBinY = 100;

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
        spdlog::error("Mask plane - 5 parameters required, {} given",
                      opt_mask.GetArraySize());
        abort();
    }

    minTheta = atan(-(masklength+fibrewidth*fibrenum)*sqrt(2)/2/detectorsource)*180/M_PI+180.;
    if (opt_theta.GetArraySize() == 2) {
        minTheta = opt_theta.GetDoubleArrayValue(1);
        maxTheta = opt_theta.GetDoubleArrayValue(2);
    } else if (opt_theta.GetArraySize() != 0) {
        spdlog::error("Theta angles - 2 parameters required: min and max values, {} given",
                      opt_theta.GetArraySize());
        abort();
    }
    if (opt_source.GetArraySize() == 2) {
        xDimSource = opt_source.GetDoubleArrayValue(1);
        yDimSource = xDimSource;
        maxBinX = opt_source.GetIntArrayValue(2);
        maxBinY = maxBinX;
    } else if (opt_source.GetArraySize() != 0) {
        spdlog::error("Source position - 2 parameters required, {} given",
                      opt_source.GetArraySize());
        abort();
    }

    printf("Detector : %g %g %i %g [mm]\n", detectorsource, fibrenum*fibrewidth, fibrenum, fibrewidth);
    printf("Mask     : %g %g %g %g [mm]\n", masksource, masklength, masklength, maskthick);
    printf("Mask order      : %i\n", mord);
    printf("No. of events  : %i\n", opt_events.GetIntValue());
    printf("Energy [keV] : %i\n", opt_energy.GetIntValue());
    printf("Theta [Deg] : %g %g\n", minTheta, maxTheta);
    printf("Source plane [mm] : %g %g Number of bins: %i %i\n", xDimSource, yDimSource, maxBinX, maxBinY);

    double maskdetector = detectorsource-masksource;
    int energy = opt_energy.GetIntValue();
    auto material = MaterialManager::get()->LuAGCe();
    auto wrappingmaterial = MaterialManager::get()->GetMaterial("G4_Al");
    auto airmaterial = MaterialManager::get()->GetMaterial("G4_AIR");

    MuraMask mask(
        mord, {masklength * mm, masklength * mm, maskthick * mm}, 
        MaterialManager::get()->GetMaterial("G4_W"),opt_masktype.GetStringValue());
	int nLayer = 10;
    DetectorBlock detector(
        nLayer,                 // number of layers
        FibreLayer(          //
            fibrenum,             // number of fibres in layer
            Fibre({fibrewidth*fibrenum * mm, // fibre length
                   fibrewidth * mm, // fibre width and thickness
                   material,
                   wrappingmaterial,
                   airmaterial})));

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

    storage.setBinnedSize(maxBinX, maxBinY, fibrenum, fibrenum, fibrewidth);
    storage.resizeHmatrix();

    storage.newSimulation(
            TString::Format(
                "%g_%g_%d", maskdetector, masksource, energy),false);
    
    storage.writeMetadata("energy", energy * keV);
    storage.writeMetadata(
        "sourceToMaskDistance", masksource * mm);
    storage.writeMetadata(
        "maskToDetectorDistance", maskdetector * mm);
    detector.writeMetadata(&storage);
    mask.writeMetadata(&storage);
    storage.writeMetadata("sourceMinX", - xDimSource / 2);
    storage.writeMetadata("sourceMaxX", xDimSource / 2);
    storage.writeMetadata("sourceMinY", - yDimSource / 2);
    storage.writeMetadata("sourceMaxY", yDimSource / 2);
    storage.writeMetadata("sourceBinX", maxBinX);
    storage.writeMetadata("sourceBinY", maxBinY);
    storage.init();

    log::info("maskDetDistance {}, maskSrcDistance {}",maskdetector * mm, masksource * mm);
    construction->setMaskPos(masksource * mm);
    construction->setDetectorPos(detectorsource * mm + nLayer*fibrewidth/2 * mm);
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
                    "baseEnerg{}",
                    sPosX * mm,
                    sPosY * mm,
                    maskdetector * mm,
                    masksource * mm,
                    energy_it * keV);
                storage.setCurrentBins(binX, binY);

                source.SetPos(TVector3(sPosX, sPosY, 0));
                runManager.BeamOn(nIter);
                break;
            }
        }
    }
    storage.writeHmatrix();
    storage.cleanup();
}
