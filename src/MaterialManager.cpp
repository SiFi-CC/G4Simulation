#include "MaterialManager.h"

namespace SiFi
{

MaterialManager* MaterialManager::instance = nullptr;

G4Material* MaterialManager::GetMaterial(const G4String& materialName)
{
    if (fMaterial[materialName] == nullptr)
    {
        fMaterial[materialName] = G4NistManager::Instance()->FindOrBuildMaterial(materialName);
    }
    return fMaterial[materialName];
}

G4Material* MaterialManager::Vacuum()
{
    if (fVacuum == nullptr)
    {
        fVacuum = new G4Material("Vacuum", 1., 1.01 * g / mole, universe_mean_density, kStateGas,
                                 2.73 * kelvin, 3.e-18 * pascal);
    }
    return fVacuum;
}

G4Material* MaterialManager::LuAGCe()
{
    if (fLuAGCe == nullptr)
    {
        fLuAGCe = new G4Material("LuAGCe", 6.73 * g / cm3, 4);
        fLuAGCe->AddElement(fElements["Lutetium"], 61.51 * perCent);
        fLuAGCe->AddElement(fElements["Aluminium"], 15.88 * perCent);
        fLuAGCe->AddElement(fElements["Oxygen"], 22.47 * perCent);
        fLuAGCe->AddElement(fElements["Cerium"], 0.14 * perCent);
    }
    return fLuAGCe;
}

G4Material* MaterialManager::LYSO()
{
    if (fLYSO == nullptr)
    {
        fLYSO = new G4Material("LYSO", 7.1 * g / cm3, 5);
        fLYSO->AddElement(fElements["Lutetium"], 55.04 * perCent);
        fLYSO->AddElement(fElements["Yttrium"], 27.97 * perCent);
        fLYSO->AddElement(fElements["Silicon"], 4.4 * perCent);
        fLYSO->AddElement(fElements["Oxygen"], 12.49 * perCent);
        fLYSO->AddElement(fElements["Cerium"], 0.1 * perCent);
    }
    return fLYSO;
}

} // namespace SiFi
