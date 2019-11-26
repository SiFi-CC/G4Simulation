// #ifndef __Simulation_H_
// #define __Simulation_H_ 1
// #include "CLog.hh"
// #include "Source.hh"
// #include "Track.hh"
// #include <Mask.hh>
// #include <TFile.h>
// #include <TGeoManager.h>
// #include <TH2F.h>
// #include <TMatrix.h>
// #include <TTree.h>
#include "Source.hh"
#include "geometry/MuraMask.h"
#include "geometry/DetectorBlock.h"


#include <G4RunManager.hh>

using namespace std;

namespace SiFi {
class Simulation {

public:
  // Simulation() = default;
  Simulation(Source* source, MuraMask* mask, DetectorBlock* detector, DataStorage* storage)
      : fSource_VU(source), fMask_VU(mask), fDetector_VU(detector), fStorage_VU(storage) {
    // spdlog::info("Hi");
    // Init();
  }
  // virtual ~Simulation();
  void Init();
  void RunSim(int nEvents, bool singlesimulationmode = true);

  void callme(int i){
    std::cout<< "Launched by thread " << i <<std::endl;
  }
  // void ResetSimulation();

  // void Write(TString name) const;
  // void Print() const;

  // TH2F* GetObject() const { return fH2Source; };
  // TH2F* GetImage() const { return fH2Detector; };

  // void SetLogLevel(spdlog::level::level_enum level) { log->set_level(level); }

private:
  // void Init();
  // Bool_t ProcessEvent();
  // void BuildTGeometry(TString name) const;

  /** Source of radiation used in simulation */
  Source* fSource_VU = nullptr;
  /** Mask implementation */
  MuraMask* fMask_VU = nullptr;
  /** Detector plane */
  DetectorBlock* fDetector_VU = nullptr;

  DataStorage* fStorage_VU;

  G4RunManager* runManager = nullptr;

  // /** Stores data of simulated particles. Particle is saved only if projected
  //  * path is crossing mask and detector, otherwise is not scored anywhere.
  //  */
  // TTree* fTree;
  // /** Histogram representing distribution of tracks in Source */
  // TH2F* fH2Source;
  // /** Histogram representing distribution of tracks in detector. This image is
  //  * used in reconstruction
  //  *
  //  * TODO: This data will be moved to Detector in the future.
  //  */
  // TH2F* fH2Detector;
  // /** Distribution of theta angles of registered particles
  //  */
  // TH1F* fH1Theta;

  // * Distribution of phi angles of registered particles
   
  // TH1F* fH1Phi;

  // *
  //  * Information about currently processed particle, needs to be kept in
  //  * object because TTree api requires source of data to be defined on init.
  //  * fTree object is Filled usng fields of this struct.
   
  // struct {
  //   /** Tracks representing paricle state at source mask an detector plane */
  //   Track sourceTrack, maskTrack, detectorTrack;
  //   /** Flag that describes whether particle was absorbed by mask */
  //   Bool_t absorbed;
  // } fPersist;

  // SiFi::logger log = SiFi::createLogger("CMSimulation");

  // ClassDef(Simulation, 0)
};
}  
// #endif