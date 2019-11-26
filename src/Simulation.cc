#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PhysicsList.h"
#include "PrimaryGeneratorAction.h"
#include "tracking/SteppingAction.h"

#include "Simulation.hh"
#include "Source.hh"

#include <TParameter.h>
#include <TFile.h>
#include <TString.h>
#include <TVector2.h>

#include <G4RunManager.hh>

using namespace std;

namespace SiFi {

void Simulation::Init(){

    spdlog::info("Xpos: {}, Ypos: {}", fSource_VU->GetPos().X(), fSource_VU->GetPos().Y());
    spdlog::info("Detector: {}", fDetector_VU->getPosition());
    spdlog::info("Mask: {}", fMask_VU->getPosition());
    



    auto construction = new DetectorConstruction(fMask_VU, fDetector_VU);
    construction->setMaskPos(fMask_VU->getPosition() * cm);
    construction->setDetectorPos(fDetector_VU->getPosition() * cm);

    runManager = new G4RunManager;
    runManager->SetUserInitialization(construction);
    auto physicsList = new PhysicsList();
    runManager->SetUserInitialization(physicsList);

    fSource_VU->Init();

    runManager->SetUserAction(new PrimaryGeneratorAction(fSource_VU->GetSource()));
    runManager->SetUserAction(new SteppingAction(fStorage_VU));
    runManager->SetUserAction(new EventAction(fStorage_VU));
    runManager->Initialize();


}



void Simulation::RunSim(int nEvents, bool singlesimulationmode){
	
    const int nIter = nEvents;

    spdlog::info(
        "Starting simulation source({}, {}), "
        "sourceToDetectorDistance={}, sourceToMaskDistance={}, "
        "baseEnergy={}",
        fSource_VU->GetPos().X() * cm,
        fSource_VU->GetPos().Y() * cm,
        fDetector_VU->getPosition() * cm,
        fMask_VU->getPosition() * cm,
        fSource_VU->GetEnergy() * keV);

    // fStorage_VU->newSimulation(
    //     TString::Format(
    //         "%g_%g", fSource_VU->GetPos().X(), fSource_VU->GetPos().Y()),
    //     singlesimulationmode);
    fStorage_VU->newSimulation(
        TString::Format(
            "%g_%g_%g_%g_%d_%d", fSource_VU->GetPos().X(), fSource_VU->GetPos().Y(),
                             fDetector_VU->getPosition(), fMask_VU->getPosition(),
                              fSource_VU->GetEnergy(),nIter),
        singlesimulationmode);

    fStorage_VU->writeMetadata("sourcePosX", fSource_VU->GetPos().X() * cm);
    fStorage_VU->writeMetadata("sourcePosY", fSource_VU->GetPos().Y() * cm);
    fStorage_VU->writeMetadata("sourcePosZ", 0 * cm);
    fStorage_VU->writeMetadata("energy", fSource_VU->GetEnergy() * keV);
    fStorage_VU->writeMetadata("sourceToMaskDistance", fMask_VU->getPosition() * cm);
    fStorage_VU->writeMetadata("maskToDetectorDistance", fDetector_VU->getPosition() * cm);
    fDetector_VU->writeMetadata(fStorage_VU);
    fMask_VU->writeMetadata(fStorage_VU);
    fStorage_VU->init(); 

    runManager->BeamOn(nIter);
    fStorage_VU->cleanup();
}
}