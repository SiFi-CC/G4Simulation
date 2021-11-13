#pragma once

#include "geometry/DetectorBlock.h"
#include "geometry/HypMedBlock.h"
#include "geometry/Crystal.h"
#include "geometry/CrystalLayer.h"
#include "geometry/MuraMask.h"

namespace SiFi
{
namespace defaults
{

namespace geometry
{

Crystal simpleCrystal();
CrystalLayer simpleCrystalLayer();
DetectorBlock simpleDetectorBlock();
MuraMask simpleMask();

} // namespace geometry

namespace material
{

G4Material* mask();
G4Material* fibre();
G4Material* wrapping();
G4Material* coupling();
G4Material* detectorFilling();

} // namespace material

} // namespace defaults
} // namespace SiFi
