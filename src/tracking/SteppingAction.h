#pragma once

#include "DataStorage.h"
#include "Utils.h"
#include <G4UserSteppingAction.hh>

namespace SiFi {

class SteppingAction : public G4UserSteppingAction {
  public:
    explicit SteppingAction(DataStorage* storage) : fStorage(storage){};

    void UserSteppingAction(const G4Step* step) override;

    const logger log = createLogger("Simulation");

  private:
    DataStorage* fStorage = nullptr;
};

} // namespace SiFi
