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
#include <TSystem.h>
#include "DataStorage.h"
#include "G4SPSRandomGenerator.hh"
#include "CmdLineConfig.hh"

namespace SiFi {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  public:
    explicit PrimaryGeneratorAction(G4GeneralParticleSource* gps, 
        DataStorage* storage) : fSource(gps), fStorage(storage) {
        // G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        // G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
        // if (particle == nullptr) {
        //     log->error("Unable to find particle in particle table");
        // }
        gps->SetParticleDefinition(fParticle);
        
        // gps->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
        gps->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
        gps->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        fParticleGun->SetParticleDefinition(fParticle);
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
        G4ThreeVector direction;
        G4ThreeVector position;

        TString filename = CmdLineOption::GetStringValue("SourceFile");
        bool point = false; 
        Float_t sPosX = 0.0;
        Float_t sPosY = 0.0;
        if (CmdLineOption::GetArraySize("Source") == 2) {
            point = true; 
            sPosX = CmdLineOption::GetDoubleArrayValue("Source", 1);
            sPosY = CmdLineOption::GetDoubleArrayValue("Source", 2);
        }
          log->info("filename {}", filename);
          TFile* file = new TFile(filename, "READ");
          TTree* ftree = (TTree*)file->Get("Secondaries");
          std::vector<double>* energy = 0;
          std::vector<TVector3>* pos;
          pos = new std::vector<TVector3>;
          int imax = ftree->GetEntries();
          std::vector<TVector3>* fDir = 0;
          std::vector<int>* id = 0;
          std::vector<double>* time = 0;
          ftree->SetBranchAddress("ParticleID", &id);
          ftree->SetBranchAddress("ParticleEnergy", &energy);
          ftree->SetBranchAddress("ParticleTime", &time);
          ftree->SetBranchAddress("ParticlePosition", &pos);
          ftree->SetBranchAddress("ParticleMomentum", &fDir);
          
          
          std::vector<G4ThreeVector>  pos_vec;
          std::vector<G4ThreeVector> mom_vec;
          std::vector<double>  en_vec;

          G4double rndm, rndm2;
          G4double px, py, pz;
          G4double costheta;
          log->info("imax {}", imax);
          for (int i = 0; i <imax; i++){ 
            if (i % 1000000 == 0){
              log->info("{}", i);
            }
            ftree->GetEntry(i);
            for (int j = 0; j < id->size(); j++)
            {
              if (id->at(j) == 22)
              {
                if(point){
                  position = G4ThreeVector(sPosX,sPosY,0);
                } else {
                  position = G4ThreeVector(pos->at(j).Y(), -25.0-pos->at(j).Z(), pos->at(j).X());
                }
                fParticleGun->SetParticlePosition(position);
                direction = G4ThreeVector(fDir->at(j).Y(), -fDir->at(j).Z(), fDir->at(j).X());
                if (direction.theta() < TMath:: Pi()/6 | (2.0 * TMath:: Pi() - direction.theta()) < TMath:: Pi()/6){
                    pos_vec.push_back(position);
                    mom_vec.push_back(direction);
                    en_vec.push_back(energy->at(j));
                }

                break;
              }
              
            }
          }
          file->Close();
          // for (int i = 0; i <pos_vec.size(); i++){
          //   log->info("{}", i);
          //   log->info("Position {} {} {}", pos_vec.at(i).getX(), pos_vec.at(i).getY(), pos_vec.at(i).getZ());
          //   log->info("Momentum {} {} {}", mom_vec.at(i).getX(), mom_vec.at(i).getY(), mom_vec.at(i).getZ());
          //   log->info("Energy {}", en_vec.at(i));
          // }
          log->info("size = {}", pos_vec.size());
          for (int i = 0; i <pos_vec.size(); i++){
            fParticleGun->SetParticleDefinition(fParticle);
            fParticleGun->SetParticlePosition(pos_vec.at(i));
            fParticleGun->SetParticleMomentumDirection(mom_vec.at(i));
            fParticleGun->SetParticleEnergy(en_vec.at(i));
            // log->info("Position {} {} {} {}", pos_vec.at(i).getX(), pos_vec.at(i).getY(), pos_vec.at(i).getZ(), en_vec.at(i));
            fStorage->registerEventStart(i, pos_vec.at(i), mom_vec.at(i), en_vec.at(i));
            fParticleGun->GeneratePrimaryVertex(event);
          }
        log->info("Done");
        
    }
    const logger log = createLogger("PrimaryGeneratorAction");

  private:
    G4GeneralParticleSource* fSource;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* fParticle = particleTable->FindParticle("gamma");
    G4ParticleGun* fParticleGun  = new G4ParticleGun(10);
    DataStorage* fStorage = nullptr;
    G4int fIStart;

};

} // namespace SiFi
