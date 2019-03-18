cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project (SiFi-G4Simulation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(Geant4 10.2.2 REQUIRED)
find_package(ROOT 6.13 REQUIRED RIO Geom)
message(STATUS "Found Geant4 version " ${Geant4_VERSION} )
message(STATUS "Found ROOT version " ${ROOT_VERSION} ) 
add_subdirectory(third_party/spdlog)


add_library(g4sim
    ./src/Simulation.cpp
    ./src/PhysicsList.cpp
    ./src/PrimaryGeneratorAction.cpp
    ./src/MuraMask.cpp
    ./src/EventAction.cpp
)

target_compile_features(g4sim PRIVATE cxx_std_17)
target_include_directories(g4sim
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        /usr/local/include/Geant4 # TODO (export target Geant)
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(g4sim
    PUBLIC 
        ${ROOT_LIBRARIES}
        spdlog
        ${Geant4_LIBRARIES}   # TODO (export target)
)

add_subdirectory(cmd)

