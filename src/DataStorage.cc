#include "DataStorage.h"

namespace SiFi
{

DataStorage::DataStorage(const TString& filename) : fFile(new TFile(filename, "RECREATE")) {}

DataStorage::DataStorage(const TString& filename, int rank)
{
    if (rank == 0) { fFile = new TFile(filename, "RECREATE"); }
}

DataStorage::~DataStorage()
{
    // fFile->Close();
    delete fFile;
}

void DataStorage::writeMetadata(TObject* obj) { fMetadata.tree->GetUserInfo()->Add(obj); }
void DataStorage::writeMetadata(const TString& key, double value)
{
    fMetadata.tree->GetUserInfo()->Add(new TParameter<double>(key, value));
    fMetadata.data[key.Data()] = value;
}

double DataStorage::getMetadataNumber(const TString& key) { return fMetadata.data[key.Data()]; }

void DataStorage::newSimulation(bool deposits)
{
    // fFile->cd(name);
    fMetadata.tree = new TTree("metadata", "metadata");
    if (!deposits)
    {
        fEnable.depositScoring = false;
        fEnable.hMatrixScoring = true;
    }
}

void DataStorage::init(bool hypmed)
{
    if (!hypmed)
    {
        fEnergyDeposits.hits = new TTree("deposits", "energy deposits in detector");
        fEnergyDeposits.hits->Branch("position", &fEnergyDeposits.position);
        fEnergyDeposits.hits->Branch("energy", &fEnergyDeposits.energy);
        fEnergyDeposits.hits->Branch("id", &fEnergyDeposits.eventId);
        fEnergyDeposits.histogram =
            TH2F("energyDeposits", "energy deposits in detector",
                static_cast<int>(fMetadata.data["detectorBinX"]), fMetadata.data["detectorMinX"],
                fMetadata.data["detectorMaxX"], static_cast<int>(fMetadata.data["detectorBinY"]),
                fMetadata.data["detectorMinY"], fMetadata.data["detectorMaxY"]);
    }
    else
    {
        for (int nLay = 0; nLay<3; nLay++){
            std::string layerNumber = std::to_string(nLay);
            fLayersDeposits[nLay].hits = new TTree(TString("depositsLayer" + layerNumber),
                                                    "energy deposits in detector");
            fLayersDeposits[nLay].hits->Branch("position", &fLayersDeposits[nLay].position);
            fLayersDeposits[nLay].hits->Branch("energy", &fLayersDeposits[nLay].energy);
            fLayersDeposits[nLay].hits->Branch("id", &fLayersDeposits[nLay].eventId);
            fLayersDeposits[nLay].histogram =
                TH2F(TString("energyDepositsLayer" + layerNumber), "energy deposits in detector",
                     static_cast<int>(fMetadata.data["layer" + layerNumber + "BinX"]),
                     fMetadata.data["layer" + layerNumber + "MinX"],
                     fMetadata.data["layer" + layerNumber + "MaxX"],
                     static_cast<int>(fMetadata.data["layer" + layerNumber + "BinY"]),
                     fMetadata.data["layer" + layerNumber + "MinY"],
                     fMetadata.data["layer" + layerNumber + "MaxY"]);
        }
    }

    fMaskEnergyDeposits.hits = new TTree("maskDeposits", "energy deposits in mask");
    fMaskEnergyDeposits.hits->Branch("position", &fMaskEnergyDeposits.position);
    fMaskEnergyDeposits.hits->Branch("energy", &fMaskEnergyDeposits.energy);
    fMaskEnergyDeposits.hits->Branch("id", &fMaskEnergyDeposits.eventId);
    fMaskEnergyDeposits.histogram =
        TH2F("maskEnergyDeposits", "energy deposits in mask",
             static_cast<int>(fMetadata.data["maskBinX"]), fMetadata.data["maskMinX"],
             fMetadata.data["maskMaxX"], static_cast<int>(fMetadata.data["maskBinY"]),
             fMetadata.data["maskMinY"], fMetadata.data["maskMaxY"]);
    fSourceRecord.events = new TTree("source", "source events");
    fSourceRecord.events->Branch("position", &fSourceRecord.position);
    fSourceRecord.events->Branch("direction", &fSourceRecord.direction);
    fSourceRecord.events->Branch("energy", &fSourceRecord.energy);
    fSourceRecord.events->Branch("id", &fSourceRecord.eventId);
    fSourceRecord.histogram = TH2F("sourceHist", "source events", fMetadata.data["sourceNBinX"],
                                   fMetadata.data["sourceMinX"], fMetadata.data["sourceMaxX"],
                                   fMetadata.data["sourceNBinY"], fMetadata.data["sourceMinY"],
                                   fMetadata.data["sourceMaxY"]);
}

void DataStorage::registerDepositScoring(const G4String& volume, const G4ThreeVector& pos,
                                         double energy)
{
    if (volume == "fibrephysical")
    {
        if (fEnable.depositScoring)
        {   
            fEnergyDeposits.eventId = fSourceRecord.eventId;
            fEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
            fEnergyDeposits.energy = energy;

            fEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
            // fEnergyDeposits.hits->Fill();
            total_deposited += energy;
            // spdlog::info("Energy = {}", energy);
        }
        if (fEnable.hMatrixScoring) { setHmatrix(pos.x(), pos.y(), fBinX, fBinY, energy); }
        return;
    }
    if (volume == "maskBin" && fEnable.maskDepositScoring)
    {
        fMaskEnergyDeposits.eventId = fSourceRecord.eventId;
        fMaskEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
        fMaskEnergyDeposits.energy = energy;

        fMaskEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
        fMaskEnergyDeposits.hits->Fill();
        return;
    }
}

void DataStorage::registerDepositScoringHypMed(const G4String& volume, const G4int layerNum,
                                               const G4ThreeVector& pos, double energy)
{
    if (volume == "crystalphysical")
    {
        if (fEnable.depositScoring)
        {
            fLayersDeposits[layerNum].eventId = fSourceRecord.eventId;
            fLayersDeposits[layerNum].position = TVector3(pos.x(), pos.y(), pos.z());
            fLayersDeposits[layerNum].energy = energy;

            fLayersDeposits[layerNum].histogram.Fill(pos.x(), pos.y(), energy);
            // fLayersDeposits[layerNum].hits->Fill();
            total_deposited += energy;
        }
        if (fEnable.hMatrixScoring) { setHmatrixHypMed(layerNum, pos.x(), pos.y(), fBinX, fBinY, energy); }
        return;
    }
    if (volume == "maskBin" && fEnable.maskDepositScoring)
    {
        fMaskEnergyDeposits.eventId = fSourceRecord.eventId;
        fMaskEnergyDeposits.position = TVector3(pos.x(), pos.y(), pos.z());
        fMaskEnergyDeposits.energy = energy;

        fMaskEnergyDeposits.histogram.Fill(pos.x(), pos.y(), energy);
        fMaskEnergyDeposits.hits->Fill();
        return;
    }
}

void DataStorage::registerEventStart(int eventId, const G4ThreeVector& pos,
                                     const G4ThreeVector& dir, double energy)
{
    fSourceRecord.eventId = eventId;
    fSourceRecord.position = TVector3(pos.x(), pos.y(), pos.z());
    fSourceRecord.direction = TVector3(dir.x(), dir.y(), dir.z());
    fSourceRecord.energy = energy;

    // first event should be saved every time to preserve info about
    // position of point source
    if (fEnable.sourceRecord || eventId == 0)
    {
        fSourceRecord.histogram.Fill(pos.x(), pos.y(), energy);
        // fSourceRecord.events->Fill();
    }
}

void DataStorage::resizeHmatrix() { fMatrixH.ResizeTo(fDetBinsX * fDetBinsY, fMaxBinX * fMaxBinY); }

void DataStorage::resizeHmatrixHypMed()
{
    for (int i = 0; i < 3; i++)
    {
        fMatrixHHypMed[i].ResizeTo(fDetBinsXLayers[i] * fDetBinsYLayers[i],
                                   fMaxBinX * fMaxBinY);
    }
}

void DataStorage::setHmatrix(double DetX, double DetY, int sourceBinX, int sourceBinY,
                             double energy)
    {
    auto sourceHistBin = std::make_tuple<int, int>(std::forward<int>(sourceBinX + 1),
                                                   std::forward<int>(sourceBinY + 1));

    auto sourceMatBin = std::make_tuple<int, int>(fMaxBinY - std::get<1>(sourceHistBin),
                                                  std::get<0>(sourceHistBin) - 1);
    int colIndexMatrixH = std::get<1>(sourceMatBin) * fMaxBinY + std::get<0>(sourceMatBin);

    double x = DetX;
    double y = DetY;
    auto nBinX = static_cast<int>((x + fDetBinsX * fDetBinSize / 2) / fDetBinSize) + 1;
    auto nBinY = static_cast<int>((y + fDetBinsX * fDetBinSize / 2) / fDetBinSize) + 1;
    nBinX = nBinX < 1 ? 1 : nBinX;
    nBinX = nBinX > fDetBinsX - 1 ? fDetBinsX : nBinX;
    nBinY = nBinY < 1 ? 1 : nBinY;
    nBinY = nBinY > fDetBinsY - 1 ? fDetBinsY : nBinY;
    int rowIndexMatrixH = (nBinX - 1) * fDetBinsY + fDetBinsY - nBinY;
    // spdlog::info("x = {}, y = {}", x, y);//nBinY - HISTOBIN
    // spdlog::info("rowIndexMatrixH = {}, colIndexMatrixH = {}", rowIndexMatrixH,
    // colIndexMatrixH);//nBinY - HISTOBIN fMatrixH(rowIndexMatrixH,colIndexMatrixH) ++;
    fMatrixH(rowIndexMatrixH, colIndexMatrixH) += energy;
    // fMatrixH(rowIndexMatrixH, colIndexMatrixH) += 1;
}

void DataStorage::setHmatrixHypMed(int layerNum, double DetX, double DetY,
                             int sourceBinX, int sourceBinY,
                             double energy)
{
    auto sourceHistBin = std::make_tuple<int, int>(std::forward<int>(sourceBinX + 1),
                                                   std::forward<int>(sourceBinY + 1));

    auto sourceMatBin = std::make_tuple<int, int>(fMaxBinY - std::get<1>(sourceHistBin),
                                                  std::get<0>(sourceHistBin) - 1);
    int colIndexMatrixH = std::get<1>(sourceMatBin) * fMaxBinY + std::get<0>(sourceMatBin);

    double x = DetX;
    double y = DetY;
    auto nBinX = fLayersDeposits[layerNum].histogram.GetXaxis()->FindBin(x);
    auto nBinY = fLayersDeposits[layerNum].histogram.GetYaxis()->FindBin(y);
    // auto nBinX = static_cast<int>((x + fDetBinsXLayers[layerNum] * fDetBinSize / 2) / fDetBinSize) + 1;
    // auto nBinY = static_cast<int>((y + fDetBinsYLayers[layerNum] * fDetBinSize / 2) / fDetBinSize) + 1;
    // spdlog::info("xbin : hist {}, loc {}", nBinCheckX, nBinX);//nBinY - HISTOBIN
    // spdlog::info("ybin : hist {}, loc {}", nBinCheckY, nBinY);//nBinY - HISTOBIN
    nBinX = nBinX < 1 ? 1 : nBinX;
    nBinX = nBinX > fDetBinsXLayers[layerNum] - 1 ? fDetBinsXLayers[layerNum] : nBinX;
    nBinY = nBinY < 1 ? 1 : nBinY;
    nBinY = nBinY > fDetBinsYLayers[layerNum] - 1 ? fDetBinsYLayers[layerNum] : nBinY;
    int rowIndexMatrixH = (nBinX - 1) * fDetBinsYLayers[layerNum] + fDetBinsYLayers[layerNum] - nBinY;
    // spdlog::info("x = {}, y = {}", x, y);//nBinY - HISTOBIN
    // spdlog::info("Layer {}", layerNum); // nBinY - HISTOBIN
    // spdlog::info("xbin {}, ybin {}", nBinX, nBinY); // nBinY - HISTOBIN
    // spdlog::info("rowIndexMatrixH = {}, colIndexMatrixH = {}", rowIndexMatrixH, colIndexMatrixH);
    //nBinY - HISTOBIN fMatrixH(rowIndexMatrixH,colIndexMatrixH) ++;
    fMatrixHHypMed[layerNum](rowIndexMatrixH, colIndexMatrixH) += energy;
}

void DataStorage::writeHmatrix()
{
    for (int i = 0; i < fMatrixH.GetNcols(); i++)
    {
        double sum = 1.0;
        // double sum = 0.0;
        // for(int j = 0; j < fMatrixH.GetNrows(); j ++){
        //     sum += fMatrixH(j,i);
        // }
        for (int j = 0; j < fMatrixH.GetNrows(); j++)
        {
            fMatrixH(j, i) = fMatrixH(j, i) == 0 ? 1e-9 : fMatrixH(j, i) / sum;
        }
    }
    fMatrixH.Write("matrixH");
}

// for the using with MPI library
void DataStorage::writeHmatrix(int world_rank, int world_size)
{
    for (int i = 0; i < fMatrixH.GetNcols(); i++)
    {
        double sum = 1.0;
        // double sum = 0.0;
        // for(int j = 0; j < fMatrixH.GetNrows(); j ++){
        //     sum += fMatrixH(j,i);
        // }
        for (int j = 0; j < fMatrixH.GetNrows(); j++)
        {
            fMatrixH(j, i) = fMatrixH(j, i) == 0 ? 1e-9 : fMatrixH(j, i) / sum;
        }
    }

    TMatrixT<Double_t> fMatrixH2;
    if (world_rank == 0) { fMatrixH2.ResizeTo(fDetBinsX * fDetBinsY, fMaxBinX * fMaxBinY); }
    for (int rank = 1; rank < world_size; rank++)
    {
        spdlog::info("gathering from {} ...", world_rank);
        for (int i = 0; i < fMatrixH.GetNrows(); i++)
        {
            for (int j = 0; j < fMatrixH.GetNcols(); j++)
            {
                if (world_rank == 0)
                {
                    MPI_Recv(&fMatrixH2(i, j), 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD,
                             MPI_STATUS_IGNORE);
                    fMatrixH(i, j) += fMatrixH2(i, j);
                }
                else if (world_rank == rank)
                {
                    MPI_Ssend(&fMatrixH(i, j), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
                }
            }
        }
    }
    spdlog::info("gathered {}", world_rank);
    if (world_rank == 0) { fMatrixH.Write("matrixH"); }
}

// for the using with MPI library
void DataStorage::writeHmatrixHypMed(int world_rank, int world_size)
{   
    for (int nLay = 0; nLay <3; nLay++)
    {
        for (int i = 0; i < fMatrixHHypMed[nLay].GetNcols(); i++)
        {
            double sum = 1.0;
            // double sum = 0.0;
            // for(int j = 0; j < fMatrixHHypMed[nLay].GetNrows(); j ++){
            //     sum += fMatrixHHypMed[nLay](j,i);
            // }
            for (int j = 0; j < fMatrixHHypMed[nLay].GetNrows(); j++)
            {
                fMatrixHHypMed[nLay](j, i) = fMatrixHHypMed[nLay](j, i) == 0 ? 1e-9 : fMatrixHHypMed[nLay](j, i) / sum;
            }
        }

        std::vector<TMatrixT<Double_t>> fMatrixH2(3);
        if (world_rank == 0) {
            fMatrixH2[nLay].ResizeTo(fDetBinsXLayers[nLay] * fDetBinsYLayers[nLay],
                               fMaxBinX * fMaxBinY);
        }
        for (int rank = 1; rank < world_size; rank++)
        {
            spdlog::info("gathering from {} ...", world_rank);
            for (int i = 0; i < fMatrixHHypMed[nLay].GetNrows(); i++)
            {
                for (int j = 0; j < fMatrixHHypMed[nLay].GetNcols(); j++)
                {
                    if (world_rank == 0)
                    {
                        MPI_Recv(&fMatrixH2[nLay](i, j), 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                        fMatrixHHypMed[nLay](i, j) += fMatrixH2[nLay](i, j);
                    }
                    else if (world_rank == rank)
                    {
                        MPI_Ssend(&fMatrixHHypMed[nLay](i, j), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
                    }
                }
            }
        }
    }
    spdlog::info("gathered {}", world_rank);
    // if (world_rank == 0) { fMatrixH.Write("matrixH"); }
    for (int nLay = 0; nLay < 3; nLay++)
    {
        if (world_rank == 0) {fMatrixHHypMed[nLay].Write(TString::Format("matrixH%d", nLay)); }
    }
}

void DataStorage::cleanup()
{
    // fMatrixH.Write("matrixH");
    Printgammacount();
    fFile->Write();
    delete fEnergyDeposits.hits;
    delete fSourceRecord.events;
    delete fMaskEnergyDeposits.hits;
    delete fMetadata.tree;
}

void DataStorage::enableAll()
{
    fEnable.sourceRecord = true;
    fEnable.depositScoring = true;
    fEnable.maskDepositScoring = true;
}

void DataStorage::setCurrentBins(int binX, int binY)
{
    fBinX = binX;
    fBinY = binY;
}

void DataStorage::setBinnedSize(int sourceBinX, int sourceBinY, int detectorBinX, int detectorBinY,
                                double detectorBinSize)
{
    fMaxBinX = sourceBinX;
    fMaxBinY = sourceBinY;
    fDetBinsX = detectorBinX;
    fDetBinsY = detectorBinY;
    fDetBinSize = detectorBinSize;
}

void DataStorage::setBinnedSize(int sourceBinX, int sourceBinY, std::vector<int> detectorBinXlayers,
                                std::vector<int> detectorBinYlayers, double detectorBinSize)
{
    fMaxBinX = sourceBinX;
    fMaxBinY = sourceBinY;
    fDetBinsXLayers = detectorBinXlayers;
    fDetBinsYLayers = detectorBinYlayers;
    fDetBinSize = detectorBinSize;
}

} // namespace SiFi
