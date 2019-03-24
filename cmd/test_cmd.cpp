
#include "DataStorage.h"
#include "Simulation.h"
#include "Utils.h"
#include "geometry/DetectorBlock.h"
#include "geometry/Fibre.h"
#include "geometry/FibreLayer.h"
#include "geometry/MuraMask.h"
#include <iostream>

int main(int argc, char** argv) {
    SiFi::log::info("Start program");

    SiFi::log::info("Define simulation");
    SiFi::Simulation sim(argc, argv);

    SiFi::log::info("Define storage");
    SiFi::DataStorage storage("./simulation_results.root");
    sim.setDataStorage(&storage);
    storage.newSimulation("test");

    SiFi::log::info("Create mask");
    auto mask = SiFi::defaults::geometry::simpleMask();
    SiFi::log::info("Create detector");
    auto detector = SiFi::defaults::geometry::simpleDetectorBlock();

    SiFi::log::info("prepare simulation");
    sim.prepare(&mask, &detector);
    SiFi::log::info("run Simulation");
    sim.run(1e6);
}
