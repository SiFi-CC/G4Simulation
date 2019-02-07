#pragma once

#include <Geant4/G4VUserActionInitialization.hh>

namespace SiFi {

class ActionInitialization : public G4VUserActionInitialization {
  public:
    ActionInitialization() = default;
    void Build() const override{};
};

} // namespace SiFi
