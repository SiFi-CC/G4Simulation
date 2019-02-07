#pragma once

#include <Geant4/G4VUserPhysicsList.hh>

namespace SiFi {

class PhysicsList : public G4VUserPhysicsList {
  public:
    PhysicsList() = default;
    void ConstructParticle() override{};
    void ConstructProcess() override{};
};

} // namespace SiFi
