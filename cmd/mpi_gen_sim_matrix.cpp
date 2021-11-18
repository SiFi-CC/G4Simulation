#include <mpi.h>

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
                          "Detector-source distance [mm], default: 220",220);
    CmdLineOption opt_mask(
        "Mask", "-mask",
        "Mask: order:mask-source:width:length:thickness [mm], default: 31:150:64:20", 0, 0);
    CmdLineOption opt_masktype("MaskType", "-masktype",
                               "MaskType: {standart, round, pet, nowallpet, nowallpetcut}",
                               "standart");
    CmdLineOption opt_masktype_cut(
        "MaskCut", "-cut",
        "MaskNumber of pixels (relevant only if massktype=nowallpetcut), default: 31(integer)", 31);
    CmdLineOption opt_events("Events", "-n", "Number of events, default: 1000 (integer)", 1000);
    CmdLineOption opt_energy("Energy", "-e", "Energy of particles [keV], default: 4400 (integer)",
                             4400);
    CmdLineOption opt_source("Source", "-source",
                             "Source plane size and number of bins [mm], default: 64:100", 0);

    CmdLineArg cmdarg_output("output", "Output file", CmdLineArg::kString);

    CmdLineOption opt_precision("Precision", "-er",
                                "Error in W rods size, default: 0.0[mm]"
                                "(relevant only for pet and nowallpet masks",
                                0.0);

    CmdLineOption opt_dimension("Single_dimension", "-1d", "Run in 1 dimension");

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

    const Positional& args = CmdLineConfig::GetPositionalArguments();

    Float_t detectorsource = opt_det.GetDoubleValue(); // detector dimensions

    Int_t mord = 31;                                              // MURA mask order
    Float_t masksource = 150., masklength = 64., maskthick = 20.; // mask dimensions

    Float_t sRange = 64;                 // source dimensions
    Int_t maxBinX = 100, maxBinY = 100;  // source bins

    if (opt_mask.GetArraySize() == 4)
    {
        mord = opt_mask.GetIntArrayValue(1);
        masksource = opt_mask.GetDoubleArrayValue(2);
        masklength = opt_mask.GetDoubleArrayValue(3);
        maskthick = opt_mask.GetDoubleArrayValue(4);
    }
    else if (opt_mask.GetArraySize() != 0)
    {
        spdlog::error("Mask plane - 5 parameters required, {} given", opt_mask.GetArraySize());
        abort();
    }

    if (opt_source.GetArraySize() == 2)
    {
        sRange = opt_source.GetDoubleArrayValue(1);
        maxBinX = opt_source.GetIntArrayValue(2);
        maxBinY = maxBinX;
    }
    else if (opt_source.GetArraySize() != 0)
    {
        spdlog::error("Source position - 2 parameters required, {} given",
                      opt_source.GetArraySize());
        abort();
    }
    if (CmdLineOption::GetFlagValue("Single_dimension")) { maxBinY = 1; }


    double maskdetector = detectorsource - masksource;
    int energy = opt_energy.GetIntValue();
    auto material = MaterialManager::get()->LuAGCe();
    auto wrappingmaterial = MaterialManager::get()->GetMaterial("G4_Al");
    auto airmaterial = MaterialManager::get()->GetMaterial("G4_AIR");

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    spdlog::info("processor {}", world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    TString output;
    if (world_rank == 0)
    {
        // TString output(args.at("output")->GetStringValue());
        output = args.at("output")->GetStringValue();
    }
    else
    {
        // TString output("outputMPI"+std::to_string(world_rank)+".root");
        output = "outputMPI" + std::to_string(world_rank) + ".root";
    }
    // DataStorage storage(output);
    DataStorage storage(output, world_rank);

    // choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    // set random seed with system time
    G4long seed = time(NULL);
    CLHEP::HepRandom::setTheSeed((world_rank + 1) * seed);

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

    printf("Detector HypMed Array: %g %g %g [mm]\n", detectorsource, layer2binsY * crystalWidth,
           crystalWidth);
    printf("Mask     : %s, %g %g %g %g [mm]\n", opt_masktype.GetStringValue(), masksource,
           masklength, masklength, maskthick);
    printf("Mask order      : %i\n", mord);
    printf("No. of events  : %i\n", opt_events.GetIntValue());
    printf("Energy [keV] : %i\n", opt_energy.GetIntValue());
    printf("Source plane [mm] : %g %g Number of bins: %i %i\n", sRange, sRange, maxBinX, maxBinY);

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

    G4RunManager runManager;
    runManager.SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager.SetUserInitialization(physicsList);

    Source source(energy);

    runManager.SetUserAction(new PrimaryGeneratorAction(source.GetSource()));
    runManager.SetUserAction(new SteppingAction(&storage));
    runManager.SetUserAction(new EventAction(&storage));
    runManager.Initialize();

    const int nIter = opt_events.GetIntValue();

    storage.setBinnedSize(maxBinX, maxBinY,
                          {layer0binsX, layer1binsX, layer2binsX},
                          {layer0binsY, layer1binsY, layer2binsY},
                          crystalWidth);
    storage.resizeHmatrixHypMed();

    storage.newSimulation(false);

    storage.writeMetadata("energy", energy * keV);
    storage.writeMetadata("sourceToMaskDistance", masksource * mm);
    storage.writeMetadata("maskToDetectorDistance", maskdetector * mm);
    detector.writeMetadata(&storage);
    mask.writeMetadata(&storage);
    storage.writeMetadata("sourceMinX", -sRange / 2);
    storage.writeMetadata("sourceMaxX", sRange / 2);
    storage.writeMetadata("sourceMinY", -sRange / 2);
    storage.writeMetadata("sourceMaxY", sRange / 2);
    storage.writeMetadata("sourceNBinX", maxBinX);
    storage.writeMetadata("sourceNBinY", maxBinY);
    storage.init(true);

    log::info("maskDetDistance {}, maskSrcDistance {}", maskdetector * mm, masksource * mm);
    construction->setMaskPos(masksource * mm);
    construction->setDetectorPos(detectorsource * mm + detector.getThickness() / 2 * mm);
    runManager.DefineWorldVolume(construction->Construct());
    runManager.GeometryHasBeenModified();
    log::info("world_size = {}", world_size);

    log::debug("xDimSource = {}, yDimSource = {}",sRange,sRange);
    // log::info("maxBinX = {}, maxBinY = {}",maxBinX,maxBinY);
    for (int binX = 0; binX < maxBinX; binX += 1)
    {
        for (int binY = 0; binY < maxBinY; binY += 1)
        {
            double sPosX = -sRange / 2. + (0.5 + binX) * (sRange / maxBinX);
            double sPosY = -sRange / 2. + (0.5 + binY) * (sRange / maxBinY);
            // for (int energy_it  = energy; energy_it > 50; energy_it /= 4) {
            if (binX % world_size == world_rank)
            {
                log::info("Starting simulation source({}, {}), "
                            "maskToDetectorDistance={}, sourceToMaskDistance={}, "
                            "binX{}, processor {}",
                            sPosX * mm, sPosY * mm, maskdetector * mm, masksource * mm, binX,
                            world_rank);
                // log::info("processor {} calculates column {}", world_rank, binX);
                storage.setCurrentBins(binX, binY);

                source.SetPosAng(TVector3(sPosX, sPosY, 0), layer2.getSizeY() * mm,
                                    detectorsource * mm);
                // source.SetPosAng(TVector3(sPosX, sPosY, 0));
                runManager.BeamOn(nIter);
                // break;
            }
            // }
        }
    }
    // storage.writeHmatrix(world_rank, world_size);
    storage.writeHmatrixHypMed(world_rank, world_size);
    if (world_rank == 0) { storage.cleanup(); }
    MPI_Finalize();
    // storage.gather(output);
}
