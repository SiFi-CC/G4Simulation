#pragma once

#include "Utils.h"
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
        fFile->Close();
        delete fFile;
    };

    void writeMetadata(TObject* obj) { fMetadata->GetUserInfo()->Add(obj); };

    void newSimulation(const TString& name) {
        fFile->mkdir(name);
        fFile->cd(name);

        fMetadata = new TTree("metadata", "metadata");

        fEnergyDeposits.hits = new TTree("deposits", "energy deposits in detector");
        fEnergyDeposits.hits->Branch("position", &fEnergyDeposits.position);
        fEnergyDeposits.hits->Branch("energy", &fEnergyDeposits.energy);
        fEnergyDeposits.hits->Branch("id", &fEnergyDeposits.eventId);
        fEnergyDeposits.histogram = TH2F(
            "energyDepostions",
            "energyDepostions",
            100,
            -15 * cm,
            15 * cm,
            100,
            -15 * cm,
            15 * cm);

        fMaskEnergyDeposits.hits = new TTree("maskDeposits", "energy deposits in mask");
        fMaskEnergyDeposits.hits->Branch("position", &fMaskEnergyDeposits.position);
        fMaskEnergyDeposits.hits->Branch("energy", &fMaskEnergyDeposits.energy);
        fMaskEnergyDeposits.hits->Branch("id", &fMaskEnergyDeposits.eventId);
        fMaskEnergyDeposits.histogram = TH2F(
            "maskEnergydeposits",
            "mask energy deposits",
            100,
            -15 * cm,
            15 * cm,
            100,
            -15 * cm,
            15 * cm);

        fSourceRecord.events = new TTree("source", "source events");
        fSourceRecord.events->Branch("position", &fSourceRecord.position);
        fSourceRecord.events->Branch("direction", &fSourceRecord.direction);
        fSourceRecord.events->Branch("energy", &fSourceRecord.energy);
        fSourceRecord.events->Branch("id", &fSourceRecord.eventId);
        fSourceRecord.histogram = TH2F(
            "source_hist",
            "source events",
            100,
            -15 * cm,
            15 * cm,
            100,
            -15 * cm,
            15 * cm);
    }

    void registerDepositScoring(
        const G4String& volume, int eventId, const G4ThreeVector& pos, double energy) {
        if (volume == "fibreLayerRepFibre") {
            fEnergyDeposits.eventId = eventId;
            fEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
            fEnergyDeposits.energy = energy;

            fEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
            fEnergyDeposits.hits->Fill();
            return;
        }
        if (volume == "maskBin") {
            fMaskEnergyDeposits.eventId = eventId;
            fMaskEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
            fMaskEnergyDeposits.energy = energy;

            fMaskEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
            fMaskEnergyDeposits.hits->Fill();
            return;
        }
    }

    void registerEventStart(
        int eventId, const G4ThreeVector& pos, const G4ThreeVector& dir, double energy) {
        fSourceRecord.eventId = eventId;
        fSourceRecord.position = TVector3(pos.x(), pos.y(), pos.z());
        fSourceRecord.direction = TVector3(dir.x(), dir.y(), dir.z());
        fSourceRecord.energy = energy;

        fSourceRecord.histogram.Fill(pos.x(), pos.y(), energy);
        fSourceRecord.events->Fill();
    }

    void cleanup() {
        fFile->Write();
        delete fEnergyDeposits.hits;
        delete fSourceRecord.events;
        delete fMaskEnergyDeposits.hits;
        delete fMetadata;
    }

  private:
    TFile* fFile;
    TTree* fMetadata = nullptr;

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
        TTree* hits = nullptr;

        TVector3 position;
        int eventId = 0;
        double energy = 0;

        TH2F histogram;
    } fMaskEnergyDeposits;

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
