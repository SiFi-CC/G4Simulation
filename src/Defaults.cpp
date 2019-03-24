#include "Defaults.h"
#include "MaterialManager.h"
#include <Geant4/G4SystemOfUnits.hh>

namespace SiFi {
namespace defaults {
namespace geometry {

Fibre simpleFibre() {
    return Fibre({.length = 20. * cm,
                  .width = 2. * cm,
                  .thickness = 1. * cm,
                  .fibreMaterial = material::fibre(),
                  .wrappingMaterial = material::wrapping(),
                  .couplingMaterial = material::coupling()});
};

FibreLayer simpleFibreLayer() { return {10, simpleFibre()}; };

DetectorBlock simpleDetectorBlock() { return {5, simpleFibreLayer()}; };

MuraMask simpleMask() {
    return MuraMask(11, {20. * cm, 20. * cm, 3. * cm}, material::mask());
};

} // namespace geometry

namespace material {

G4Material* mask() { return MaterialManager::get()->GetMaterial("G4_W"); };
G4Material* fibre() { return MaterialManager::get()->GetMaterial("G4_W"); };
G4Material* wrapping() { return MaterialManager::get()->GetMaterial("G4_W"); };
G4Material* coupling() { return MaterialManager::get()->GetMaterial("G4_W"); };
G4Material* detectorFilling() { return MaterialManager::get()->GetMaterial("G4_W"); };

} // namespace material

} // namespace defaults
} // namespace SiFi
