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

#include <mpi.h>


#include "DataStructConvert.hh"



namespace SiFi {

class DataStorage {
  public:
    explicit DataStorage(const TString& filename, int rank);
    explicit DataStorage(const TString& filename);

    virtual ~DataStorage();
    void writeMetadata(TObject* obj);
    void writeMetadata(const TString& key, double value);
    
    double getMetadataNumber(const TString& key);

    void newSimulation(const TString& name, bool deposits = true);

    // should be run after metadata
    void init();

    void registerDepositScoring(
        const G4String& volume, const G4ThreeVector& pos, double energy);

    void registerEventStart(
        int eventId, const G4ThreeVector& pos, const G4ThreeVector& dir, double energy);

    void resizeHmatrix();

    void setHmatrix(double DetX,double DetY, int sourceBinX, int sourceBinY);

    void writeHmatrix();
    void writeHmatrix(TString str);
    
    void cleanup();

    void enableAll();

    void setCurrentBins(int binX, int binY);

    void setBinnedSize(int sourceBinX, int sourceBinY, int detectorBinX, int detectorBinY, double detectorBinSize);

  protected:
    TFile* fFile = nullptr;

    int fBinX, fBinY,fMaxBinX, fMaxBinY, fDetBinsX,fDetBinsY;
    double fDetBinSize;
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
        bool hMatrixScoring = false;
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
