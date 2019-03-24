#include "MaterialManager.h"

namespace SiFi {

MaterialManager* MaterialManager::instance = nullptr;

G4Material* MaterialManager::GetMaterial(const G4String& materialName) {
    if (fCache[materialName] == nullptr) {
        fCache[materialName] =
            G4NistManager::Instance()->FindOrBuildMaterial(materialName);
    }
    return fCache[materialName];
};

G4Material* MaterialManager::Vacuum() {
    if (fVacuum == nullptr) {
        fVacuum = new G4Material(
            "Vacuum",
            1.,
            1.01 * g / mole,
            universe_mean_density,
            kStateGas,
            2.73 * kelvin,
            3.e-18 * pascal);
    }
    return fVacuum;
}

} // namespace SiFi
