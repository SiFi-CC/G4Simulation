#pragma once

#include "Utils.h"
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <TFile.h>
#include <TH2F.h>
#include <TParameter.h>
#include <TTree.h>
#include <TVector3.h>
#include <math.h>
#include <string>
#include <unordered_map>

#include <mpi.h>

#include "CmdLineConfig.hh"
#include "DataStructConvert.hh"

namespace SiFi
{

struct fLayerDeposits
{
    TTree* hits = nullptr;

    TVector3 position;
    int eventId = 0;
    double energy = 0;

    TH2F histogram;
};

class DataStorage
{
public:
    explicit DataStorage(const TString& filename, int rank);
    explicit DataStorage(const TString& filename);

    virtual ~DataStorage();
    void writeMetadata(TObject* obj);
    void write(TObject obj){obj.Write();};
    void writeMetadata(const TString& key, double value);

    double getMetadataNumber(const TString& key);

    void newSimulation(bool deposits = true);

    // should be run after metadata
    void init(bool hypmed = false);

    void registerDepositScoring(const G4String& volume, const G4ThreeVector& pos, double energy,
                                int fiberID);
    
    void registerDepositScoringHypMed(const G4String& volume, const G4int layerNum,
                                const G4ThreeVector& pos, double energy);

    void registerEventStart(int eventId, const G4ThreeVector& pos, const G4ThreeVector& dir,
                            double energy);

    void resizeHmatrix();
    
    void resizeHmatrixHypMed();

    void setHmatrix(double DetX, double DetY, int sourceBinX, int sourceBinY, double energy);
    
    void setHmatrix1D(int fibreNo, int sourceBinX, int sourceBinY, double energy);
    
    void setHmatrixHypMed(int layerNum, double DetX, double DetY,
                    int sourceBinX, int sourceBinY, double energy);

    void writeHmatrix();
    void writeHmatrix(int world_rank, int world_size);

    void writeHmatrixHypMed(int world_rank, int world_size);

    void gather(TString output);

    void cleanup();

    void enableAll();

    void setCurrentBins(int binX, int binY);

    void setBinnedSize(int sourceBinX, int sourceBinY, int detectorBinX, int detectorBinY,
                       double detectorBinSize);

    void setBinnedSize(int sourceBinX, int sourceBinY,
                       std::vector<int> detectorBinXlayers, std::vector<int> detectorBinYlayers,
                       double detectorBinSize);

    void gammacount()
    {
        fGammaCount++;
        // std::cout<<"GammaCount = " << fGammaCount << std::endl;
    }

    void enablesource()
    {
        fEnable.sourceRecord = true;
        fEnable.maskDepositScoring = false;
    }

    void enableHypMed()
    {
        fEnable.hypmed = true;
    }

    void Printgammacount()
    {
        // fGammaCount ++;
        std::cout << "GammaCount = " << fGammaCount << std::endl;
        std::cout << "TotalDeposited = " << total_deposited << std::endl;
    }

protected:
    TFile* fFile = nullptr;

    //current bins of the source plane
    int fBinX, fBinY;
    // number of bins in the source plane
    int fMaxBinX, fMaxBinY;
    // number of bins in the detector
    int fDetBinsX, fDetBinsY;
    //number of bins in each layer (for hypmed)
    std::vector<int> fDetBinsXLayers, fDetBinsYLayers;
    int fGammaCount = 0;
    double fDetBinSize;
    double total_deposited = 0;
    TMatrixT<Double_t> fMatrixH;
    std::vector<TMatrixT<Double_t>> fMatrixHHypMed = {TMatrixT<Double_t>(), TMatrixT<Double_t>(),
                                                      TMatrixT<Double_t>()};

    struct
    {
        TTree* tree = nullptr;
        std::unordered_map<std::string, double> data = {
            {"detectorMinX", -150}, {"detectorMaxX", 150}, {"detectorMinY", -150},
            {"detectorMaxY", 150},  {"detectorBinX", 100}, {"detectorBinY", 100},
            {"maskMinX", -150},     {"maskMaxX", 150},     {"maskMinY", -150},
            {"maskMaxY", 150},      {"maskBinX", 100},     {"maskBinY", 100},
            {"sourceNBinX", 100},   {"sourceNBinY", 100},  {"sourceMinX", -150},
            {"sourceMaxX", 150},    {"sourceMinY", -150},  {"sourceMaxY", 150}};
    } fMetadata;

    struct
    {
        bool sourceRecord = false;
        bool depositScoring = true;
        bool maskDepositScoring = false;
        bool hMatrixScoring = false;
        bool hypmed = false;
    } fEnable;

    struct
    {
        TTree* hits = nullptr;

        TVector3 position;
        int eventId = 0;
        int fiberId = 0;
        double energy = 0;

        TH2F histogram;
    } fEnergyDeposits;

    struct
    {
        TTree* hits = nullptr;

        TVector3 position;
        int eventId = 0;
        double energy = 0;

        TH2F histogram;
    } fMaskEnergyDeposits;

    struct
    {
        TTree* events = nullptr;

        TVector3 position;
        TVector3 direction;
        int eventId = 0;
        double energy = 0;

        TH2F histogram;
    } fSourceRecord;

    std::vector<fLayerDeposits> fLayersDeposits = {fLayerDeposits(), fLayerDeposits(),
                                                   fLayerDeposits()};
};

} // namespace SiFi
