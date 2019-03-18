#pragma once

#include "Utils.h"
#include <Geant4/G4VUserPhysicsList.hh>

namespace SiFi {

class PhysicsList : public G4VUserPhysicsList {
  public:
    PhysicsList() = default;
    void ConstructParticle() override;
    void ConstructProcess() override;

    const logger log = createLogger("PhysicsList");
};

} // namespace SiFi
