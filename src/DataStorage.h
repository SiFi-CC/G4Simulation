#pragma once

#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4ThreeVector.hh>
#include <TFile.h>
#include <TH2F.h>
#include <TTree.h>
#include <TVector3.h>

namespace SiFi {

class DataStorage {
  public:
    explicit DataStorage(const TString& filename)
        : fFile(new TFile(filename, "RECREATE")){};

    virtual ~DataStorage() {
        fFile->Write();
        fFile->Close();
        delete fFile;
    };

    void NewSimulation(const TString& name) {
        Cleanup();
        fFile->mkdir(name);
        fFile->cd(name);

        fEnergyDeposits.hits = new TTree("deposits", "energy deposits in detector");
        fEnergyDeposits.hits->Branch("position", &fEnergyDeposits.position);
        fEnergyDeposits.hits->Branch("energy", &fEnergyDeposits.energy);
        fEnergyDeposits.hits->Branch("id", &fEnergyDeposits.eventId);
        fEnergyDeposits.histogram = TH2F(
            "energyDepostions",
            "energyDepostions",
            100,
            -50 * cm,
            50 * cm,
            100,
            -50 * m,
            50 * cm);

        fSourceRecord.events = new TTree("source", "source events");
        fSourceRecord.events->Branch("position", &fSourceRecord.position);
        fSourceRecord.events->Branch("direction", &fSourceRecord.direction);
        fSourceRecord.events->Branch("energy", &fSourceRecord.energy);
        fSourceRecord.events->Branch("id", &fSourceRecord.eventId);
        fSourceRecord.histogram = TH2F(
            "source_hist",
            "source events",
            100,
            -50 * cm,
            50 * cm,
            100,
            -50 * m,
            50 * cm);
    }

    void RegisterFirstDepositScoring(
        const G4String& volume, int eventId, const G4ThreeVector& pos, double energy) {
        if (volume != "fibreLayerRepFibre") {
            return;
        }
        fEnergyDeposits.eventId = eventId;
        fEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
        fEnergyDeposits.energy = energy;

        fEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
        fEnergyDeposits.hits->Fill();
    }

    void RegisterEventStart(
        int eventId, const G4ThreeVector& pos, const G4ThreeVector& dir, double energy) {
        fSourceRecord.eventId = eventId;
        fSourceRecord.position = TVector3(pos.x(), pos.y(), pos.z());
        fSourceRecord.direction = TVector3(dir.x(), dir.y(), dir.z());
        fSourceRecord.energy = energy;

        fSourceRecord.histogram.Fill(pos.x(), pos.y(), energy);
        fSourceRecord.events->Fill();
    }

    void Cleanup() {
        delete fEnergyDeposits.hits;
        delete fSourceRecord.events;
    }

  private:
    TFile* fFile;
    struct {
        bool sourceRecord = false;
        bool idealScoring = false;
        bool opticalPhotonsScoring = false;
        bool firstDepositScoring = false;
        bool depositScoring = true;
    } fEnable;

    struct {
        TTree* hits = nullptr;

        TVector3 position;
        int eventId = 0;
        double energy = 0;

        TH2F histogram;
    } fEnergyDeposits;

    struct {
        TTree* events = nullptr;

        TVector3 position;
        TVector3 direction;
        int eventId = 0;
        double energy = 0;

        TH2F histogram;
    } fSourceRecord;
};

} // namespace SiFi
