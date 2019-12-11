#include "Defaults.h"
#include "MaterialManager.h"
#include <G4SystemOfUnits.hh>

namespace SiFi {
namespace defaults {
namespace geometry {

Fibre simpleFibre() {
    return Fibre({20. * cm,
                  1. * cm,
                  1. * cm, // thickness
                  material::fibre(),
                  material::wrapping(),
                  material::coupling()});
}

FibreLayer simpleFibreLayer() { return {20, simpleFibre()}; }

DetectorBlock simpleDetectorBlock() { return {5, simpleFibreLayer()}; }

MuraMask simpleMask() {
    return MuraMask(11, {20. * cm, 20. * cm, 2. * cm}, material::mask());
}

} // namespace geometry

namespace material {

G4Material* mask() { return MaterialManager::get()->GetMaterial("G4_W"); }
G4Material* fibre() { return MaterialManager::get()->LuAGCe(); }
G4Material* wrapping() { return MaterialManager::get()->GetMaterial("G4_W"); }
G4Material* coupling() { return MaterialManager::get()->GetMaterial("G4_W"); }
G4Material* detectorFilling() { return MaterialManager::get()->GetMaterial("G4_W"); }

} // namespace material

} // namespace defaults
} // namespace SiFi

