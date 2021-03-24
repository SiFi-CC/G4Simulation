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
#include "G4SPSRandomGenerator.hh"

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
        // fParticleGun->SetParticleEnergy(4400 * keV);
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
        G4ThreeVector direction;
        G4ThreeVector position;
        TFile* file = new TFile("output9.root", "READ");
        TTree* ftree = (TTree*)file->Get("Secondaries");
        // int imax;
        std::vector<double>* energy = 0;
        std::vector<TVector3>* pos;
        pos = new std::vector<TVector3>;
        int imax = ftree->GetEntries();
        // log->info("imax {}", imax);
        std::vector<TVector3>* fDir = 0;
        std::vector<int>* id = 0;
        std::vector<double>* time = 0;
        ftree->SetBranchAddress("ParticleID", &id);
        ftree->SetBranchAddress("ParticleEnergy", &energy);
        ftree->SetBranchAddress("ParticleTime", &time);
        ftree->SetBranchAddress("ParticlePosition", &pos);
        ftree->SetBranchAddress("ParticleMomentum", &fDir);

        G4double rndm, rndm2;
        G4double px, py, pz;
        G4double costheta;
        log->info("imax {}", imax);
        for (int i = 0; i <imax; i++)
        { 
          if (i % 1000000 == 0){
            log->info("{}", i);
          }
          ftree->GetEntry(i);
          for (int j = 0; j < energy->size(); j++)
          {
            if (id->at(j) == 22)
            {
              position = G4ThreeVector(pos->at(j).Y(), -25.0-pos->at(j).Z(), pos->at(j).X());
              // position = G4ThreeVector(0,0,0);
              fParticleGun->SetParticlePosition(position);
              direction = G4ThreeVector(fDir->at(j).Y(), -fDir->at(j).Z(), fDir->at(j).X());
              if (direction.theta() < TMath:: Pi()/12 | (2.0 * TMath:: Pi() - direction.theta()) < TMath:: Pi()/12){
                fParticleGun->SetParticleMomentumDirection(direction);
                fParticleGun->SetParticleEnergy(energy->at(j));
                fStorage->registerEventStart(event->GetEventID(), position, direction, energy->at(j));
                // if(-pos->at(j).Z() < 50.0){
                fParticleGun->GeneratePrimaryVertex(event);
                // }
              }

              break;
            }
            
          }
          
          // if ((fDir->X() != 0 || fDir->Y() != 0 || fDir->Z() != 0) && pos->Z()>-150){
          // position = G4ThreeVector(pos->Y(), -pos->Z(), pos->X());
          // direction = G4ThreeVector(fDir->Y(), -fDir->Z(), fDir->X());
          // fParticleGun->SetParticlePosition(position);
          // fParticleGun->SetParticleMomentumDirection(direction);
          // fParticleGun->SetParticleEnergy(energy);
          // fStorage->registerEventStart(event->GetEventID(), position, direction, energy);
          // fParticleGun->GeneratePrimaryVertex(event);
          // }
        }
        log->info("Done");
        
    }
    const logger log = createLogger("PrimaryGeneratorAction");

  private:
    G4GeneralParticleSource* fSource;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    G4ParticleGun* fParticleGun  = new G4ParticleGun(10);
    DataStorage* fStorage = nullptr;

};

} // namespace SiFi
