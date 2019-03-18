#pragma once

#include <Geant4/G4UserEventAction.hh>
#include <TVector3.h>
#include <TVectorT.h>

namespace SiFi {

class EventAction : public G4UserEventAction {
  public:
    EventAction() = default;
    void BeginOfEventAction(const G4Event* evt) override;
    void EndOfEventAction(const G4Event* evt) override;

  private:
    struct {
        TVector3 sourcePosition;
        TVector3 sourceDirection;
        double initialEnergy;
        std::vector<TVector3> hits;
    } eventData;
};

} // namespace SiFi
