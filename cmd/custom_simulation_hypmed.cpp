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
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace SiFi;

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::info);

    CmdLineOption opt_det("Plane", "-det",
                          "Detector-source distance [mm], default: 220", 0);
    CmdLineOption opt_mask(
        "Mask", "-mask",
        "Mask: order:mask-source:width/length:thickness [mm], default: 31:170:70:20", 0, 0);
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
                             0);
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

    CmdLineOption opt_json("Json", "-json", "Json config file", "");

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

    const Positional& args = CmdLineConfig::GetPositionalArguments();

    TString output(args.at("output")->GetStringValue());
    DataStorage storage(output);

    Float_t detectorsource = 220.;

    Int_t mord = 31;                                              // MURA mask order
    Float_t masksource = 170., masklength = 70., maskthick = 20.; // mask dimensions

    Float_t minTheta, maxTheta = 180;

    Float_t sPosX = 0, sPosY = 0; // source coordinates

    Int_t sNbins = 100; // source histogram parameters
    Float_t sRange = 70;
    Int_t energy = 4400;

    storage.enablesource();              // enblesource histogram


    if (opt_json.GetStringValue())
    {
        std::ifstream f(opt_json.GetStringValue());
        json data = json::parse(f);
        std::cout << "JSON" << std::endl;
        std::cout << data << std::endl;
        // std::cout << data.at("pi") << std::endl;
        // for (auto& el : data.items())
        // {
        //     std::cout << "key: " << el.key() << ", value:" << el.value() << ", size:" << el.value().size()
        //             << '\n';
        // }
        std::cout << data.contains("mask") << '\n';
        std::cout << data.contains("detector") << '\n';
        // std::cout << data.items() << std::endl;
        if (data.contains("mask"))
        {
            if ( data.at("mask").size() == 4)
            {
                std::cout << data.at("mask") << '\n';
                mord = (int) data.at("mask").at(0);
                masksource = (double) data.at("mask").at(1);
                masklength = (double) data.at("mask").at(2);
                maskthick = (double) data.at("mask").at(3);
            }
            else
            {
                spdlog::error("Mask plane - 4 parameters required, {} given",
                              data.at("mask").size());
                abort();
            }
        }
        if (data.contains("source"))
        {
            if (data.at("source").size() == 2)
            {
                sPosX = (double)data.at("source").at(0);
                sPosY = (double)data.at("source").at(1);
            }
            else
            {
                spdlog::error("Source position - 2 parameters required, {} given",
                              data.at("source").size());
                abort();
            }
        }
        if (data.contains("sourceBins"))
        {
            if (data.at("sourceBins").size() == 2)
            {
                sPosX = (double)data.at("sourceBins").at(0);
                sPosY = (double)data.at("sourceBins").at(1);
            }
            else
            {
                spdlog::error("Source histogram - 2 parameters required: range and Nbins, {} given",
                              data.at("sourceBins").size());
                abort();
            }
        }
        if (data.contains("energy"))
        {
            energy = (int) data.at("energy");
        }
        if (data.contains("det"))
        {
            detectorsource = (double) data.at("det");
        }
    }

    { // CmdLine options
        if (opt_mask.GetArraySize() == 4)
        {
            mord = opt_mask.GetIntArrayValue(1);
            masksource = opt_mask.GetDoubleArrayValue(2);
            masklength = opt_mask.GetDoubleArrayValue(3);
            maskthick = opt_mask.GetDoubleArrayValue(4);
        }
        else if (opt_mask.GetArraySize() != 0)
        {
            spdlog::error("Mask plane - 4 parameters required, {} given", opt_mask.GetArraySize());
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
        if (opt_energy.GetIntValue() > 0)
        {   
            energy = opt_energy.GetIntValue();
        }
        if (opt_det.GetDoubleValue() > 0)
        {   
            detectorsource = opt_det.GetIntValue();
        }
    } // CmdLine options


    // choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    // set random seed with system time
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed(seed);

    double maskdetector = (detectorsource - masksource);
    // auto material = MaterialManager::get()->LuAGCe();
    auto material = MaterialManager::get()->LYSO();
    auto wrappingmaterial = MaterialManager::get()->GetMaterial("G4_Al");
    auto airmaterial = MaterialManager::get()->GetMaterial("G4_AIR");

    MuraMask mask(mord, {masklength * mm, masklength * mm, maskthick * mm},
                  MaterialManager::get()->GetMaterial("G4_W"), opt_masktype.GetStringValue());

    // # HypMed
    double crystalWidth = 1.333;
    double layer0Z = 3.2;
    double layer1Z = 4.4;
    double layer2Z = 7.4;
    int layer0binsX = 31;
    int layer0binsY = 35;
    int layer1binsX = 31;
    int layer1binsY = 36;
    int layer2binsX = 34;
    int layer2binsY = 36;

    // minTheta =
    // atan(-(masklength+fibrewidth*fibrenum)*sqrt(2)/2/detectorsource)*180/M_PI+180.;
    minTheta =
        atan(-(masklength + layer2binsY * crystalWidth) / 2 / detectorsource) * 180 / M_PI + 180.;
    if (opt_theta.GetArraySize() == 1) { minTheta = opt_theta.GetDoubleArrayValue(1); }
    else if (opt_theta.GetArraySize() != 0)
    {
        spdlog::error("Theta_Min angle - 1 parameter is required, {} given",
                        opt_theta.GetArraySize());
        abort();
    }


    printf("Detector HypMed Array: %g %g %g [mm]\n", detectorsource, layer2binsY * crystalWidth,
           crystalWidth);
    printf("Mask     : %s, %g %g %g %g [mm]\n", opt_masktype.GetStringValue(), masksource,
           masklength, masklength, maskthick);
    printf("Mask order      : %i\n", mord);
    printf("No. of events  : %i\n", opt_events.GetIntValue());
    printf("Energy [keV] : %i\n", energy);
    printf("Theta [Deg] : %g %g\n", minTheta, maxTheta);
    printf("Source position [mm] : %g %g\n", sPosX, sPosY);
    abort();

    // top
    CrystalLayer layer0 = CrystalLayer(layer0binsX, layer0binsY,   // number of crystals in layer
                                       Crystal({crystalWidth * mm, // fibre length
                                                crystalWidth * mm, // fibre width
                                                layer0Z * mm,      // fibre thickness
                                                material, wrappingmaterial, airmaterial}));
    // middle
    CrystalLayer layer1 = CrystalLayer(layer1binsX, layer1binsY,   // number of crystals in layer
                                       Crystal({crystalWidth * mm, // fibre length
                                                crystalWidth * mm, // fibre width
                                                layer1Z * mm,      // fibre thickness
                                                material, wrappingmaterial, airmaterial}));
    CrystalLayer layer2 = CrystalLayer(layer2binsX, layer2binsY,   // number of crystals in layer
                                       Crystal({crystalWidth * mm, // fibre length
                                                crystalWidth * mm, // fibre width
                                                layer2Z * mm,      // fibre thickness
                                                material, wrappingmaterial, airmaterial}));

    HypMedBlock detector(layer0, layer1, layer2);

    auto construction = new DetectorConstruction(&mask, &detector);
    construction->setMaskPos(masksource * mm);
    construction->setDetectorPos(detectorsource * mm + detector.getThickness() / 2. * mm);

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager.SetUserInitialization(physicsList);

    Source source(energy, minTheta, maxTheta);

    // source.SetPosAng(TVector3(sPosX, sPosY, 0),fibrewidth*fibrenum *mm,detectorsource * mm);
    source.SetPosAng(TVector3(sPosX, sPosY, 0));

    runManager.SetUserAction(new PrimaryGeneratorAction(source.GetSource()));
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
    storage.init(true);

    if (opt_sourceMac.GetStringValue())
    {
        std::string str = "/control/execute ";
        str.append(opt_sourceMac.GetStringValue());

        G4UImanager* UI = G4UImanager::GetUIpointer();
        UI->ApplyCommand(str);
    }

    // if (CmdLineOption::GetFlagValue("Visualization") || data.value("vis", false))
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
