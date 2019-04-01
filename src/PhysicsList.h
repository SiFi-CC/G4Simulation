#pragma once

#include "Utils.h"
#include <Geant4/G4VModularPhysicsList.hh>

namespace SiFi {

class PhysicsList : public G4VModularPhysicsList {
  public:
    PhysicsList();
    void ConstructParticle() override;

    const logger log = createLogger("PhysicsList");
};

} // namespace SiFi
