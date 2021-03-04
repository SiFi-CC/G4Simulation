#pragma once

#include "Utils.h"
#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4VUserPrimaryGeneratorAction.hh>

#include <G4ThreeVector.hh>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TMath.h>
#include "DataStorage.h"

namespace SiFi {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  public:
    explicit PrimaryGeneratorAction(G4GeneralParticleSource* gps, 
        DataStorage* storage) : fSource(gps), fStorage(storage) {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
        if (particle == nullptr) {
            log->error("Unable to find particle in particle table");
        }
        gps->SetParticleDefinition(particle);
        
        // gps->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
        gps->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
        gps->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(4400 * keV);
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));
        
    };

    void GeneratePrimaries(G4Event* event) override {
        // log->debug("GeneratePrimaries");
        // fSource->GeneratePrimaryVertex(event);
        // log->debug("GeneratePrimaries end");
        // for (int i = 0; i < 1000; i++)
        // {
        // fParticleGun->GeneratePrimaryVertex(event);
        // }

        // }
        TFile* file = new TFile("PMMA180MeV0mmBPType3_20200603.root", "READ");
        TTree* ftree = (TTree*)file->Get("Events");
        int imax;
        int fEventNumber;
        Double_t energy;
        TVector3* pos = 0;
        G4ThreeVector position;
        imax = ftree->GetEntries();
        log->info("imax {}", imax);
        TVector3* fDir = 0;
        G4ThreeVector direction;
        ftree->SetBranchAddress("EventNumber", &fEventNumber);
        ftree->SetBranchAddress("Energy_Primary", &energy);
        ftree->SetBranchAddress("RealPosition_source", &pos);
        ftree->SetBranchAddress("RealDirection_source", &fDir);
        for (int i = 0; i <imax; i++)
        { 
          if (i % 1000000 == 0){
            log->info("{}", i);
          }
          // std::cout<<"Hello"<<std::endl;
          ftree->GetEntry(i);
          if ((fDir->X() != 0 || fDir->Y() != 0 || fDir->Z() != 0) && pos->Z()>-150){
          position = G4ThreeVector(pos->Y(), -pos->Z(), pos->X());
          direction = G4ThreeVector(fDir->Y(), -fDir->Z(), fDir->X());
          fParticleGun->SetParticlePosition(position);
          fParticleGun->SetParticleMomentumDirection(direction);
          fParticleGun->SetParticleEnergy(energy);
          fStorage->registerEventStart(event->GetEventID(), position, direction, energy);
          fParticleGun->GeneratePrimaryVertex(event);
          }
        }
        log->info("Done");
        
    }
    const logger log = createLogger("PrimaryGeneratorAction");

  private:
    G4GeneralParticleSource* fSource;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    G4ParticleGun* fParticleGun  = new G4ParticleGun(1);
    DataStorage* fStorage = nullptr;

};

} // namespace SiFi
