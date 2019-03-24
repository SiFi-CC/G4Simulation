#pragma once

#include "geometry/DetectorBlock.h"
#include "geometry/Fibre.h"
#include "geometry/FibreLayer.h"
#include "geometry/MuraMask.h"

namespace SiFi {
namespace defaults {

namespace geometry {

Fibre simpleFibre();
FibreLayer simpleFibreLayer();
DetectorBlock simpleDetectorBlock();
MuraMask simpleMask();

} // namespace geometry

namespace material {

G4Material* mask();
G4Material* fibre();
G4Material* wrapping();
G4Material* coupling();
G4Material* detectorFilling();

} // namespace material

} // namespace defaults
} // namespace SiFi
