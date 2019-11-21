#pragma once

#include "DetectorBlock.h"
#include <G4ThreeVector.hh>

namespace SiFi {
namespace geometry {

DetectorBlock detectorBlock(const G4ThreeVector& size);

} // namespace geometry
} // namespace SiFi
