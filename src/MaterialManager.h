#pragma once

#include <Geant4/G4NistManager.hh>
#include <Geant4/G4PhysicalConstants.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <string>
#include <unordered_map>

namespace SiFi {

class MaterialManager {
  public:
    static MaterialManager* get() {
        if (instance == nullptr) {
            instance = new MaterialManager();
        }
        return instance;
    };
    G4Material* GetMaterial(const G4String& materialName);
    G4Material* Vacuum();

  private:
    static MaterialManager* instance;
    MaterialManager() = default;

    G4Material* fVacuum = nullptr;
    std::unordered_map<std::string, G4Material*> fCache;
};

} // namespace SiFi
