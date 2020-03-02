#include "DataStorage.h"


namespace SiFi{

DataStorage::DataStorage(const TString& filename)
                :fFile(new TFile(filename, "RECREATE")){}

DataStorage::DataStorage(const TString& filename, int rank){
    if(rank == 0){
        fFile = new TFile(filename, "RECREATE");
    }
}


DataStorage::~DataStorage() {
        // fFile->Close();
        delete fFile;
    }

void DataStorage::writeMetadata(TObject* obj) { fMetadata.tree->GetUserInfo()->Add(obj); }
void DataStorage::writeMetadata(const TString& key, double value) {
    fMetadata.tree->GetUserInfo()->Add(new TParameter<double>(key, value));
    fMetadata.data[key.Data()] = value;
}

double DataStorage::getMetadataNumber(const TString& key) { return fMetadata.data[key.Data()]; }

void DataStorage::newSimulation(const TString& name, bool deposits) {
    // fFile->cd(name);
    fMetadata.tree = new TTree("metadata", "metadata");
    if (!deposits){
        fEnable.depositScoring = false;
        fEnable.hMatrixScoring = true;
    }
}

void DataStorage::init() {
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

void DataStorage::registerDepositScoring(
    const G4String& volume, const G4ThreeVector& pos, double energy) {
    if (volume == "fibrephysical") {
        if(fEnable.depositScoring){
            fEnergyDeposits.eventId = fSourceRecord.eventId;
            fEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
            fEnergyDeposits.energy = energy;

            fEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
            fEnergyDeposits.hits->Fill();
        }
        if(fEnable.hMatrixScoring){
            setHmatrix(pos.x(),pos.y(), fBinX, fBinY);
        }
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

void DataStorage::registerEventStart(
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

void DataStorage::resizeHmatrix(){
    fMatrixH.ResizeTo(fDetBinsX*fDetBinsY, fMaxBinX*fMaxBinY);
}

void DataStorage::setHmatrix(double DetX,double DetY, int sourceBinX, int sourceBinY){
    auto sourceHistBin = std::make_tuple<int, int>(std::forward<int>(sourceBinX+1),
        std::forward<int>(sourceBinY+1));
    
    auto sourceMatBin =
        std::make_tuple<int, int>(fMaxBinY - std::get<1>(sourceHistBin),
                                  std::get<0>(sourceHistBin) - 1);
    int colIndexMatrixH =
        std::get<1>(sourceMatBin) * fMaxBinY  + std::get<0>(sourceMatBin);

        double x = DetX;
        double y = DetY;
    auto nBinX = static_cast<int>((x + fDetBinsX*fDetBinSize/2) / fDetBinSize) + 1;
    auto nBinY = static_cast<int>((y + fDetBinsX*fDetBinSize/2) / fDetBinSize) + 1;
    nBinX = nBinX < 1 ? 1 : nBinX;
    nBinX = nBinX > fDetBinsX-1 ? fDetBinsX : nBinX;
    nBinY = nBinY < 1 ? 1 : nBinY;
    nBinY = nBinY > fDetBinsY - 1 ? fDetBinsY : nBinY;
    int rowIndexMatrixH = (nBinX-1) * fDetBinsY + fDetBinsY-nBinY;
    // spdlog::info("x = {}, y = {}", x, y);//nBinY - HISTOBIN
    // spdlog::info("rowIndexMatrixH = {}, colIndexMatrixH = {}", rowIndexMatrixH, colIndexMatrixH);//nBinY - HISTOBIN
    fMatrixH(rowIndexMatrixH,colIndexMatrixH)++;
}

void DataStorage::writeHmatrix(){
    for(int i = 0; i < fMatrixH.GetNcols(); i ++){
        double sum = 0.0;
        for(int j = 0; j < fMatrixH.GetNrows(); j ++){
            sum += fMatrixH(j,i);
        }
        for(int j = 0; j < fMatrixH.GetNrows(); j ++){
             fMatrixH(j,i) = fMatrixH(j,i) == 0 ? 1e-9 : fMatrixH(j,i)/sum;
        }
    }
    fMatrixH.Write("matrixH");
    // fFile->Close();
}

void DataStorage::writeHmatrix(TString str){
    for(int i = 0; i < fMatrixH.GetNcols(); i ++){
        double sum = 0.0;
        for(int j = 0; j < fMatrixH.GetNrows(); j ++){
            sum += fMatrixH(j,i);
        }
        for(int j = 0; j < fMatrixH.GetNrows(); j ++){
             fMatrixH(j,i) = fMatrixH(j,i) == 0 ? 1e-9 : fMatrixH(j,i)/sum;
        }
    }
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    TMatrixT<Double_t> fMatrixH2;
    fMatrixH2.ResizeTo(fDetBinsX*fDetBinsY, fMaxBinX*fMaxBinY);
    for (int rank = 1; rank < world_size; rank++){
    spdlog::info("gathering from {} ...",world_rank);
        for (int i = 0; i < fMatrixH.GetNrows(); i++){
            for (int j = 0; j < fMatrixH.GetNcols(); j++){
                if(world_rank == 0){
                            // for (int i = 0; i < fMatrixH2.GetNrows(); i++){
                    //     for (int j = 0; j < fMatrixH2.GetNcols(); j++){
                            MPI_Recv(&fMatrixH2(i,j), 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD,
                                 MPI_STATUS_IGNORE);
                    //     }
                    // }
                    fMatrixH(i,j) += fMatrixH2(i,j);
                    // spdlog::info("receiving from {} ...", fMatrixH(i,j));
                } else if (world_rank == rank) {
                    // spdlog::info("sending from {} ...", world_rank);
                    MPI_Ssend(&fMatrixH(i,j), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
                }
            }
        }    
    }
    spdlog::info("gathered {}",world_rank);
    if(world_rank == 0){
        fMatrixH.Write("matrixH");
    }

}

void DataStorage::gather(TString output){
    TFile* file = new TFile(output,"UPDATE");
    file->cd();
    TMatrixT<Double_t> matrixH, matrixHtmp;
    matrixH.Read("matrixH");
    file->Close();

    TFile* filetmp;

    int world_size = 2;
    // MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    for (int i = 1; i < world_size; i++){
        filetmp = new TFile(TString("outputMPI"+std::to_string(i)+".root"), "READ");
        // filetmp = new TFile("outputMPI1.root", "READ");
        filetmp->cd();
        matrixHtmp.Read("matrixH");
        filetmp->Close();
        // remove("outputMPI"+std::to_string(i)+".root");
        matrixH += matrixHtmp;
    }
    spdlog::info("hmat {}", matrixH(1,1));
    file = new TFile("new.root","UPDATE");
    file->cd();
    matrixH.Write("matrixH");
    file->Close();
}


void DataStorage::cleanup() {
    // fMatrixH.Write("matrixH");
    fFile->Write();
    delete fEnergyDeposits.hits;
    delete fSourceRecord.events;
    delete fMaskEnergyDeposits.hits;
    delete fMetadata.tree;
}

void DataStorage::enableAll() {
    fEnable.sourceRecord = true;
    fEnable.depositScoring = true;
    fEnable.maskDepositScoring = true;
}

void DataStorage::setCurrentBins(int binX, int binY){
    fBinX = binX;
    fBinY = binY;
}

void DataStorage::setBinnedSize(int sourceBinX, int sourceBinY,
                             int detectorBinX, int detectorBinY, double detectorBinSize){
    fMaxBinX = sourceBinX;
    fMaxBinY = sourceBinY;
    fDetBinsX = detectorBinX;
    fDetBinsY = detectorBinY;
    fDetBinSize = detectorBinSize;
}


} //namespace SiFi