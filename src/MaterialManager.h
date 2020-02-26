#pragma once

#include <G4NistManager.hh>
#include <G4PhysicalConstants.hh>
#include <G4SystemOfUnits.hh>
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
    G4Material* LuAGCe();
    G4Material* LYSO();

  private:
    static MaterialManager* instance;
    MaterialManager() = default;

    G4Material* fVacuum = nullptr;
    G4Material* fLuAGCe = nullptr;
    G4Material* fLYSO = nullptr;
    std::unordered_map<std::string, G4Material*> fMaterial = {};
    std::unordered_map<std::string, G4Element*> fElements = {
        {"Hydrogen", new G4Element("Hydrogen", "H", 1., 1.00794 * g / mole)},
        {"Carbon", new G4Element("Carbon", "C", 6., 12.0107 * g / mole)},
        {"Nitrogen", new G4Element("Nitrogen", "N", 7., 14.0067 * g / mole)},
        {"Oxygen", new G4Element("Oxygen", "O", 8., 15.9994 * g / mole)},
        {"Fluorine", new G4Element("Fluorine", "F", 9., 18.9984032 * g / mole)},
        {"Aluminium", new G4Element("Aluminium", "Al", 13., 26.9815386 * g / mole)},
        {"Titanium", new G4Element("Titanium", "Ti", 22., 47.867 * g / mole)},
        {"Lead", new G4Element("Lead", "Pb", 82., 207.2 * g / mole)},
        {"Lutetium", new G4Element("Lutetium", "Lu", 71., 174.97 * g / mole)},
        {"Cerium", new G4Element("Cerium", "Ce", 58., 140.11 * g / mole)},
        {"Praseodym", new G4Element("Praseodym", "Praseodym", 59., 140.907 * g / mole)},
        {"Gadolinium", new G4Element("Gadolinium", "Gadolinium", 64., 157.25 * g / mole)},
        {"Gallium", new G4Element("Gallium", "Gallium", 31., 69.723 * g / mole)},
        {"Magnesium", new G4Element("Magnesium", "Magnesium", 12., 24.305 * g / mole)},
        {"Yttrium", new G4Element("Yttrium", "Yttrium", 39., 88.905 * g / mole)},
        {"Silicon", new G4Element("Silicon", "Silicon", 14., 28.085 * g / mole)}};
}; // namespace SiFi

} // namespace SiFi
