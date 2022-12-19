cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project (SiFi-G4Simulation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # TODO(resolve issue with spdlog and ROOT)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_MODULE_PATH
    $ENV{ROOTSYS}
    ${CMAKE_SOURCE_DIR}/cmake-scripts
)

find_package(Geant4 REQUIRED ui_all vis_all)
find_package(ROOT REQUIRED RIO Geom MathCore Physics Gui Tree)
find_package(CmdLineArgs 2.2.0 REQUIRED)
find_package(MPI REQUIRED)
message(STATUS "Found Geant4 version " ${Geant4_VERSION} )
message(STATUS "Found ROOT version " ${ROOT_VERSION} ) 
message(STATUS "Found CmdLineArgs version " ${CmdLineArgs_VERSION} ) 
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

add_subdirectory(third_party/spdlog)
include_directories(SYSTEM third_party/spdlog/include)

include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})
include_directories(SYSTEM ${ROOT_INCLUDE_DIRS})

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

# load macros
include(c++-standards)
include(code-coverage)
include(sanitizers)

cxx_17()
add_code_coverage()

# libraries and bianries

# include_directories(BEFORE src)

add_library(g4sim
    # ./src/Defaults.cpp
    ./src/PhysicsList.cpp
    ./src/EventAction.cpp
    ./src/MaterialManager.cpp
    ./src/DataStructConvert.cc
    ./src/DataStorage.cc
    ./src/Source.cc

    ./src/geometry/DetectorBlock.cpp
    ./src/geometry/FibreLayer.cpp
    ./src/geometry/FibreLayer_Scatterrer.cpp
    ./src/geometry/Fibre.cpp
    ./src/geometry/MuraMask.cpp
    ./src/geometry/HypMedBlock.cpp
    ./src/geometry/CrystalLayer.cpp
    ./src/geometry/Crystal.cpp

    ./src/tracking/SteppingAction.cpp
)

target_include_directories(g4sim
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    PRIVATE
        /usr/local/include/Geant4 # TODO (export target Geant)
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(g4sim
    PUBLIC 
    #  ROOT::Core ROOT::RIO ROOT::Geom ROOT::MathCore ROOT::Physics ROOT::Gui ROOT::Tree
    spdlog::spdlog
    ${ROOT_LIBRARIES} 
    ${Geant4_LIBRARIES}   # TODO (export target)
)

add_subdirectory(cmd)

MESSAGE(STATUS "<<< Configuration >>>
Project:        ${CMAKE_PROJECT_NAME}

Build type      ${CMAKE_BUILD_TYPE}
Install path    ${CMAKE_INSTALL_PREFIX}

Compiler:
C               ${CMAKE_C_COMPILER}
C++             ${CMAKE_CXX_COMPILER}

Linker:
Ld              ${CMAKE_LINKER}

Compiler flags:
C               ${CMAKE_C_FLAGS}
C++             ${CMAKE_CXX_FLAGS}

Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}

Doxygen:        ${DOXYGEN_FOUND}
Build doc:      ${BUILD_DOC}
Install doc:    ${INSTALL_DOC}
\n")
