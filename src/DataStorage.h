#pragma once

#include "Utils.h"
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <TFile.h>
#include <TH2F.h>
#include <TParameter.h>
#include <TTree.h>
#include <TVector3.h>
#include <string>
#include <unordered_map>
#include <math.h>

#include "DataStructConvert.hh"



namespace SiFi {

class DataStorage {
  public:
    explicit DataStorage(const TString& filename)
        : fFile(new TFile(filename, "RECREATE")){};

    virtual ~DataStorage() {
        fFile->Close();
        delete fFile;
    };

    void writeMetadata(TObject* obj) { fMetadata.tree->GetUserInfo()->Add(obj); };
    void writeMetadata(const TString& key, double value) {
        fMetadata.tree->GetUserInfo()->Add(new TParameter<double>(key, value));
        fMetadata.data[key.Data()] = value;
    }
    double getMetadataNumber(const TString& key) { return fMetadata.data[key.Data()]; }

    void newSimulation(const TString& name, bool singleSimulationMode = false) {
        if (!singleSimulationMode) {
            fFile->mkdir(name);
            fFile->cd(name);
        } else {
            fFile->cd();
        }

        fMetadata.tree = new TTree("metadata", "metadata");
    }

    // should be run after metadata
    void init() {
        fEnergyDeposits.hits = new TTree("deposits", "energy deposits in detector");
        fEnergyDeposits.hits->Branch("position", &fEnergyDeposits.position);
        fEnergyDeposits.hits->Branch("energy", &fEnergyDeposits.energy);
        fEnergyDeposits.hits->Branch("id", &fEnergyDeposits.eventId);
        fEnergyDeposits.histogram = TH2F(
            "energyDeposits",
            "energy deposits in detector",
            static_cast<int>(fMetadata.data["detectorBinX"]),
            fMetadata.data["detectorMinX"],
            fMetadata.data["detectorMaxX"],
            static_cast<int>(fMetadata.data["detectorBinY"]),
            fMetadata.data["detectorMinY"],
            fMetadata.data["detectorMaxY"]);

        fMaskEnergyDeposits.hits = new TTree("maskDeposits", "energy deposits in mask");
        fMaskEnergyDeposits.hits->Branch("position", &fMaskEnergyDeposits.position);
        fMaskEnergyDeposits.hits->Branch("energy", &fMaskEnergyDeposits.energy);
        fMaskEnergyDeposits.hits->Branch("id", &fMaskEnergyDeposits.eventId);
        fMaskEnergyDeposits.histogram = TH2F(
            "maskEnergyDeposits",
            "energy deposits in mask",
            static_cast<int>(fMetadata.data["maskBinX"]),
            fMetadata.data["maskMinX"],
            fMetadata.data["maskMaxX"],
            static_cast<int>(fMetadata.data["maskBinY"]),
            fMetadata.data["maskMinY"],
            fMetadata.data["maskMaxY"]);

        fSourceRecord.events = new TTree("source", "source events");
        fSourceRecord.events->Branch("position", &fSourceRecord.position);
        fSourceRecord.events->Branch("direction", &fSourceRecord.direction);
        fSourceRecord.events->Branch("energy", &fSourceRecord.energy);
        fSourceRecord.events->Branch("id", &fSourceRecord.eventId);
        fSourceRecord.histogram = TH2F(
            "sourceHist",
            "source events",
            100,
            -15 * cm,
            15 * cm,
            100,
            -15 * cm,
            15 * cm);
    }

    void registerDepositScoring(
        const G4String& volume, const G4ThreeVector& pos, double energy) {
        if (volume == "fibreLayerRepFibre" && fEnable.depositScoring) {
            fEnergyDeposits.eventId = fSourceRecord.eventId;
            fEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
            fEnergyDeposits.energy = energy;

            fEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
            fEnergyDeposits.hits->Fill();

            setHmatrix(pos.x(),pos.y(), fBinX, fBinY);
            return;
        }
        if (volume == "maskBin" && fEnable.maskDepositScoring) {
            fMaskEnergyDeposits.eventId = fSourceRecord.eventId;
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

        // first event should be saved every time to preserve info about
        // position of point source
        if (fEnable.sourceRecord || eventId == 0) {
            fSourceRecord.histogram.Fill(pos.x(), pos.y(), energy);
            fSourceRecord.events->Fill();
        }
    }

    void resizeHmatrix(int maxBinX, int maxBinY){
        fMatrixH.ResizeTo(fDetBinsX*fDetBinsY, maxBinX*maxBinY);
        // fMatrixH.ResizeTo(fEnergyDeposits.histogram.GetNbinsX()*
            // fEnergyDeposits.histogram.GetNbinsY(), maxBinX*maxBinY);
    }

    void setHmatrix(double DetX,double DetY, int sourceBinX, int sourceBinY){
        auto sourceHistBin = std::make_tuple<int, int>(std::forward<int>(sourceBinX+1),
            std::forward<int>(sourceBinY+1));
        
        auto sourceMatBin =
            std::make_tuple<int, int>(fMaxBinY - std::get<1>(sourceHistBin),
                                      std::get<0>(sourceHistBin) - 1);
        int colIndexMatrixH =
            std::get<1>(sourceMatBin) * fMaxBinY  + std::get<0>(sourceMatBin);

            double x = DetX;
            double y = DetY;
        auto nBinX = static_cast<int>((x + fDetBinsX/2) / 1) + 1;
        auto nBinY = static_cast<int>((y + fDetBinsY/2) / 1) + 1;
        nBinX = nBinX < 0 ? 0 : nBinX;
        nBinX = nBinX > fDetBinsX-1 ? fDetBinsX : nBinX;
        nBinY = nBinY < 0 ? 0 : nBinY;
        nBinY = nBinY > fDetBinsY - 1 ? fDetBinsY : nBinY;
        // detMatrix(22-nBinY,nBinX-1) +=1;
        int rowIndexMatrixH = (22-nBinY) * 22 + nBinX-1;
        // std::cout << "rowIndexMatrixH: "<< rowIndexMatrixH <<
        // " colIndexMatrixH: "<<colIndexMatrixH <<std::endl;
        fMatrixH(rowIndexMatrixH,colIndexMatrixH)++;
    }

    void writeHmatrix(TString filename){

        for(int i = 0; i < fMatrixH.GetNcols(); i ++){
            double sum = 0.0;
            for(int j = 0; j < fMatrixH.GetNrows(); j ++){
                sum += fMatrixH(j,i);
            }
            for(int j = 0; j < fMatrixH.GetNrows(); j ++){
                 fMatrixH(j,i) = fMatrixH(j,i) == 0 ? 1e-9 : fMatrixH(j,i)/sum;
            }
        }

        // TFile file1(filename,"RECREATE");
        // TFile file1("my.root","RECREATE");

        // file1.cd();
        fMatrixH.Write("matrixH");
        // file1.Close();
    }
    
    void cleanup() {
        fFile->Write();
        delete fEnergyDeposits.hits;
        delete fSourceRecord.events;
        delete fMaskEnergyDeposits.hits;
        delete fMetadata.tree;
    }

    void enableAll() {
        fEnable.sourceRecord = true;
        fEnable.depositScoring = true;
        fEnable.maskDepositScoring = true;
    }
    int fBinX, fBinY,fMaxBinX, fMaxBinY, fDetBinsX,fDetBinsY; //VU MAKE private! 
  protected:
    TFile* fFile;

    TMatrixT<Double_t> fMatrixH;

    struct {
        TTree* tree = nullptr;
        std::unordered_map<std::string, double> data = {{"detectorMinX", -150},
                                                        {"detectorMaxX", 150},
                                                        {"detectorMinY", -150},
                                                        {"detectorMaxY", 150},
                                                        {"detectorBinX", 100},
                                                        {"detectorBinY", 100},
                                                        {"maskMinX", -150},
                                                        {"maskMaxX", 150},
                                                        {"maskMinY", -150},
                                                        {"maskMaxY", 150},
                                                        {"maskBinX", 100},
                                                        {"maskBinY", 100}};
    } fMetadata;

    struct {
        bool sourceRecord = false;
        bool depositScoring = true;
        bool maskDepositScoring = false;
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
