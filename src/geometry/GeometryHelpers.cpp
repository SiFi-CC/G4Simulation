#include "GeometryHelpers.h"
#include "Defaults.h"

namespace SiFi {
namespace geometry {

DetectorBlock detectorBlock(const G4ThreeVector& size) {
    return defaults::geometry::simpleDetectorBlock(); // TODO(tmp) replace with correct
                                                      // implementation
}

} // namespace geometry
} // namespace SiFi
