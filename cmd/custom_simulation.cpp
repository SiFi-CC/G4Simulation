#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "Source.hh"
#include "tracking/SteppingAction.h"

#include <TFile.h>
#include <TParameter.h>
#include <TString.h>

#include "Randomize.hh"
#include "time.h"
#include <G4RunManager.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#include <G4VisManager.hh>

#include <CmdLineConfig.hh>

#include <math.h>

using namespace SiFi;

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::info);

    CmdLineOption opt_det("Plane", "-det",
                          "Detector: detector-source:nFibres:fibre_width, default: 220:16:1.3", 0,
                          0);
    CmdLineOption opt_mask(
        "Mask", "-mask",
        "Mask: order:mask-source:xSize:ySize:thickness [mm], default: 31:170:70:70:20", 0, 0);
    CmdLineOption opt_masktype("MaskType", "-masktype",
                               "MaskType: {standart, round, pet, nowallpet, nowallpetcut}",
                               "nowallpet");
    CmdLineOption opt_masktype_cut(
        "MaskCut", "-cut",
        "MaskNumber of pixels,(relevant only if massktype=nowallpetcut) default: 31(integer)", 31);
    CmdLineOption opt_masktype_cutX(
        "MaskCutX", "-cutx",
        "MaskNumber of pixels in horizontal direction,(relevant only if massktype=nowallpetcut) default: 31(integer)", 0);
    CmdLineOption opt_masktype_cutY(
        "MaskCutY", "-cuty",
        "MaskNumber of pixels in vertical direction,(relevant only if massktype=nowallpetcut) default: 31(integer)", 0);
    CmdLineOption opt_events("Events", "-n", "Number of events, default: 1000 (integer)", 1000);
    CmdLineOption opt_energy("Energy", "-e", "Energy of particles [keV], default: 4400 (integer)",
                             4400);
    CmdLineOption opt_nlay("#layers", "-nlay", "Number of layers in detector, default: 4 (integer)",
                           4);
    CmdLineOption opt_theta("Theta", "-theta",
                            "Min Theta angle [Deg] (maximum Theta is 180), default: auto", 0);
    CmdLineOption opt_source("Source", "-source", "Source position [mm], default: 0:0", 0);
    CmdLineOption opt_sourceBins("SourceBins", "-sourceBins",
                                 "Range and number of bins and in source Histogram (is needed for "
                                 "RMSE and UQI), default: 70:100",
                                 0);
    CmdLineOption opt_sourceMac("SourceMac", "-sMac", "Source '.mac' script (string)", "");
    CmdLineOption opt_visualization("Visualization", "-vis", "Run visualization");
    CmdLineArg cmdarg_output("output", "Output file", CmdLineArg::kString);

    CmdLineOption opt_precision("Precision", "-er",
                                "Error in W rods size, default: 0.0 [mm]"
                                "(relevant only for pet and nowallpet masks",
                                0.0);

    CmdLineOption opt_dimension("Single_dimension", "-1d", "Run in 1 dimension");

    CmdLineOption opt_scat("Full_scatterrer", "-fullscat", "Full-cale scatterrer");

    CmdLineOption opt_detshift("DetectorShift", "-detshift",
                                "Detector position shift [mm], default: 0:0", 0);

    CmdLineOption opt_sourcePhase("SourcePhaseSpace", "-sFile", "tsv with source", "");
    
    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

    const Positional& args = CmdLineConfig::GetPositionalArguments();

    TString output(args.at("output")->GetStringValue());
    DataStorage storage(output);

    Float_t detectorsource = 220, fibrewidth = 2.0; // detector dimensions
    Int_t fibrenum = 16;                            // number of fibers in one layer

    Int_t mord = 31;                                              // MURA mask order
    Float_t masksource = 170., masklengthX = 70., masklengthY = 70., maskthick = 20.; // mask dimensions

    Float_t minTheta, maxTheta = 180;

    Float_t sPosX = 0, sPosY = 0; // source coordinates

    Int_t sNbins = 100; // source histogram parameters
    Float_t sRange = 70;

    double detshiftX = 0.0, detshiftY = 0.0;

    int nLayer = opt_nlay.GetIntValue(); // number of layers in the detector
    storage.enablesource();              // enblesource histogram

    { // CmdLine options
        if (opt_det.GetArraySize() == 3)
        {
            detectorsource = opt_det.GetDoubleArrayValue(1);
            fibrenum = opt_det.GetIntArrayValue(2);
            fibrewidth = opt_det.GetDoubleArrayValue(3);
        }
        else if (opt_det.GetArraySize() != 0)
        {
            spdlog::error("Detector plane - 3 parameters required, {} given",
                          opt_det.GetArraySize());
            abort();
        }
        if (opt_mask.GetArraySize() == 5)
        {
            mord = opt_mask.GetIntArrayValue(1);
            masksource = opt_mask.GetDoubleArrayValue(2);
            masklengthX = opt_mask.GetDoubleArrayValue(3);
            masklengthY = opt_mask.GetDoubleArrayValue(4);
            maskthick = opt_mask.GetDoubleArrayValue(5);
        }
        else if (opt_mask.GetArraySize() != 0)
        {
            spdlog::error("Mask plane - 4 parameters required, {} given", opt_mask.GetArraySize());
            abort();
        }

        // minTheta =
        // atan(-(masklength+fibrewidth*fibrenum)*sqrt(2)/2/detectorsource)*180/M_PI+180.;
        minTheta =
            atan(-(masklengthX + fibrewidth * fibrenum) / 2 / detectorsource) * 180 / M_PI + 180.;
        if (opt_theta.GetArraySize() == 1) { minTheta = opt_theta.GetDoubleArrayValue(1); }
        else if (opt_theta.GetArraySize() != 0)
        {
            spdlog::error("Theta_Min angle - 1 parameter is required, {} given",
                          opt_theta.GetArraySize());
            abort();
        }
        if (opt_source.GetArraySize() == 2)
        {
            sPosX = opt_source.GetDoubleArrayValue(1);
            sPosY = opt_source.GetDoubleArrayValue(2);
        }
        else if (opt_source.GetArraySize() != 0)
        {
            spdlog::error("Source position - 2 parameters required, {} given",
                          opt_source.GetArraySize());
            abort();
        }
        if (opt_sourceBins.GetArraySize() == 2)
        {
            sRange = opt_sourceBins.GetDoubleArrayValue(1);
            sNbins = opt_sourceBins.GetIntArrayValue(2);
        }
        else if (opt_sourceBins.GetArraySize() != 0)
        {
            spdlog::error("Source histogram - 2 parameters required: range and Nbins, {} given",
                          opt_sourceBins.GetArraySize());
            abort();
        }
        if (opt_detshift.GetArraySize() == 2)
        {
            detshiftX = opt_detshift.GetDoubleArrayValue(1);
            detshiftY = opt_detshift.GetDoubleArrayValue(2);
        }
        else if (opt_detshift.GetArraySize() == 1)
        {
            detshiftX = opt_detshift.GetDoubleArrayValue(1);
        }
        else if (opt_detshift.GetArraySize() != 0)
        {
            spdlog::error("Source histogram - 2 parameters required: range and Nbins, {} given",
                          opt_detshift.GetArraySize());
            abort();
        }
    } // CmdLine options

    printf("Detector : %g %g %i %g [mm]\n", detectorsource, fibrenum * fibrewidth, fibrenum,
           fibrewidth);
    printf("Mask     : %s, %g %g %g %g [mm]\n", opt_masktype.GetStringValue(), masksource,
           masklengthX, masklengthY, maskthick);
    printf("Mask order      : %i\n", mord);
    printf("No. of events  : %i\n", opt_events.GetIntValue());
    printf("Energy [keV] : %i\n", opt_energy.GetIntValue());
    printf("Theta [Deg] : %g %g\n", minTheta, maxTheta);
    printf("Source position [mm] : %g %g\n", sPosX, sPosY);

    // choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    // set random seed with system time
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed(seed);

    double maskdetector = (detectorsource - masksource);
    int energy = opt_energy.GetIntValue();
    // auto material = MaterialManager::get()->LuAGCe();
    auto material = MaterialManager::get()->LYSO();
    auto wrappingmaterial = MaterialManager::get()->GetMaterial("G4_Al");
    auto airmaterial = MaterialManager::get()->GetMaterial("G4_AIR");

    MuraMask mask(mord, {masklengthX * mm, masklengthY * mm, maskthick * mm},
                  MaterialManager::get()->GetMaterial("G4_W"), opt_masktype.GetStringValue());

    DetectorBlock detector(nLayer,                                // number of layers
                           FibreLayer(                            //
                               fibrenum,                          // number of fibres in layer
                               Fibre({100 * mm, // fibre length
                                      fibrewidth * mm,            // fibre width and thickness
                                      material, wrappingmaterial, airmaterial})));
    auto construction = new DetectorConstruction(&mask, &detector);
    construction->setMaskPos(masksource * mm);
    construction->setDetectorPos(detshiftX * mm, detshiftY * mm,
        detectorsource * mm + nLayer * fibrewidth / 2 * mm);
    spdlog::info("DetectorPosition z = {}", detectorsource * mm + nLayer * fibrewidth / 2 * mm);

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager.SetUserInitialization(physicsList);

    Source source(energy, minTheta, maxTheta);

    // source.SetPosAng(TVector3(sPosX, sPosY, 0),fibrewidth*fibrenum *mm,detectorsource * mm);
    source.SetPosAng(TVector3(sPosX, sPosY, 0));

    runManager.SetUserAction(new PrimaryGeneratorAction(source.GetSource(), &storage));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    const int nIter = opt_events.GetIntValue();

    log::info("Starting simulation source({}, {}), "
              "maskToDetectorDistance={}, sourceToMaskDistance={}, "
              "baseEnergy={}",
              sPosX * mm, sPosY * mm, maskdetector * mm, masksource * mm, energy * keV);

    storage.newSimulation(true);

    storage.writeMetadata("sourcePosX", sPosX * mm);
    storage.writeMetadata("sourcePosY", sPosY * mm);
    storage.writeMetadata("sourcePosZ", 0 * mm);
    storage.writeMetadata("energy", energy * keV);
    storage.writeMetadata("sourceToMaskDistance", masksource * mm);
    storage.writeMetadata("maskToDetectorDistance", maskdetector * mm);
    storage.writeMetadata("sourceMinX", -sRange / 2);
    storage.writeMetadata("sourceMaxX", sRange / 2);
    storage.writeMetadata("sourceMinY", -sRange / 2);
    storage.writeMetadata("sourceMaxY", sRange / 2);
    storage.writeMetadata("sourceNBinX", sNbins);
    if (CmdLineOption::GetFlagValue("Single_dimension"))
    {
        storage.writeMetadata("sourceNBinY", 1);
    }
    else
    {
        storage.writeMetadata("sourceNBinY", sNbins);
    }
    detector.writeMetadata(&storage);
    mask.writeMetadata(&storage);
    storage.init();


    if (opt_sourceMac.GetStringValue())
    {
        std::string str = "/control/execute ";
        str.append(opt_sourceMac.GetStringValue());

        G4UImanager* UI = G4UImanager::GetUIpointer();
        UI->ApplyCommand(str);
    }

    if (CmdLineOption::GetFlagValue("Visualization"))
    {
        // VISUALISATION:
        G4VisManager* visManager = new G4VisExecutive;
        visManager->Initialize();

        G4UImanager* UI = G4UImanager::GetUIpointer();

        G4UIExecutive* UI2 = new G4UIExecutive(argc, argv);
        UI->ApplyCommand("/control/execute vis_CC.mac");
        UI2->SessionStart();
        // delete UI2;
        delete visManager;
    }
    else
    {
        runManager.BeamOn(nIter);
    }

    // MULTIPLE POINTS:
    // printf("Iter");

    // source.SetPos(TVector3(5, -5, 0));
    // runManager.GeometryHasBeenModified();
    // runManager.BeamOn(nIter);

    // printf("Iter");

    //  source.SetPos(TVector3(20, 20, 0));
    // runManager.GeometryHasBeenModified();
    // runManager.BeamOn(nIter);

    storage.cleanup();
}
