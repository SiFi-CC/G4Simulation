# G4Simulation
*[SIFI-CM][sifi_link] 'simple'[^1] Geant4 Simulation*

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [General description](#general-description)
- [Usage](#usage)
  - [Simulation](#simulation)
  - [H-matrix calculation](#h-matrix-calculation)

## Prerequisites

* Required ROOT version: 6.24/00
  *For the installation guide look at [root_official][root_install]*[^root]
* Compiler supporting c++17
* CmdLineArgs >= 2.2.0
  *Available in [SiFi-CC/CmdLineArgs](https://github.com/SiFi-CC/CmdLineArgs)*
* mpich
  

## Installation

* Clone the repository
  
``` bash
git clone https://github.com/SiFi-CC/G4Simulation.git
```

``` bash
cd G4Simulation
```

* Prepare submodules (spdlog) by running:

``` bash
git submodule update --init --recursive
```

* Create build directory and compile the code

``` bash
mkdir build && cd build
cmake ..
make -jN
```

* For using visual mode one will need to copy macro files to the build directory as well
  
``` bash
cp ../*.mac .
```


## General description

The program operates in 2 modes. [The first](#simulation) of them is used to simulate an experiment of $\gamma$-particles being shot towards the detector. And [the second](#h-matrix-calculation) - for the calculation of the system matrix (needed for the reconstruction).

The general procedure is very similar for both modes. The basic difference is that the second performs a set of single simulations and combines them together in the form of `system matrix`.



## Usage

### Simulation


```shell
./cmd/custom_simulation.cp output.root [optional arguments]
```

`output.root` is a name of the output root-file.


##### Optional arguments  
*(detailed description below the table)*

| Parameter                 | Default       | Type   |  Description   |   
| :----: |:---:| :--:| :--------|
|  `-det`            |    220:16:1.3    | float:int:float | Detector parameters: detector-source:nFibres:fibre_width [mm/-/mm]
|  `-nlay`            |    4    | int  | Number of layers in detector)
|  `-mask`            |    31:170:70:20    | int:floats | Mask parameters: order:mask-source:width/length:thickness [mm]
|  `-masktype`            |    standard    | string  | Mask type: \{`standard`, `round`, `pet`, `nowallpet`, `nowallpetcut`\}
|  `-cut`            |    31    | int  | Number of pixels in mask, (relevant only if masktype=nowallpetcut)
|  `-er`            |    0.0    | float  | Error in W rods size [mm], (relevant only if `masktype`=`nowallpetcut`)
|  `-source`            |    0:0    | float:int  | Source coordinates [mm]
|  `-sourceBins`            |    70:100    | int  | Range and number of bins in the source histogram
|  `-n`            |    1000    | int  | Number of generated events
|  `-e`            |    4400    | int  | Energy of particles [keV]
|  `-theta`            |    auto    | int  | Min $\theta$ angle [Deg], (maximum $\theta$ is 180)
|  `-sMac`            |    none    | string  | mac-script to change source parameters
|  `-vis`            |    none    | flag  | Run in visual mode(no simulation is performed)
|  `-1d`            |    none    | flag  | Mask and detector are single-dimensional


**Parameter -det**

List of 3 values which define define geometry parameters, connected to the detector.
First one is `detector-source`, that is a distance between the source and the detector.
This parameter is followed by the number of fibers in the single layer and width of a single fiber.

**Parameter -nlay**

This parameter allows user to set a number of the layers in the detector.


**Parameter -mask**

List of 4 values which define define geometry parameters, connected to the `MURA mask` .
First one defines an order of the mask (should be a prime number). Second value is `mask-source`, that is a distance between the source and the mask.
The last 2 parameters are the width(and the length - as the mask is a square) and thickness.
    

**Parameter -masktype**

Specify the type of implementation of the MURA mask. It should be one of the following options:

* `standard` - ideal (theoretical) case when the mask is truly binary and has no walls between the pixels

* `round` - the mask is constructed as a Tungsten monoblock, where in places of empty pixels are round-shape
holes

* `pet` - the mask has a `PET raster` where it is possible to put Tungsten rods and prepare any pattern. The size of rods is hardcoded as a 90% of the full pixel size.

* `nowallpet` - similar to `pet` but the raster is prepared specifically to the particular pattern. It means that empty pixels are filled with PET and it allows to avoid having walls.

* `nowallpetcut` - the same as `nowallpet` but allows option `-cut` to be activated


**Parameter -cut**

If `-masktype=nowallpetcut`, the cut parameter specifies a size of mask's central area to be used. For example if mask order is 467 and `-cut=31`, then only 31X31 central pixels of the 467's order MURA mask will be used.


**Parameter -er**

Specifies the error in the size of Tungsten rods. If used, than  from the each dimension of each rod is subtracted a random number 
from the distribution: $er/2 - N(er/2, er/6)$


**Parameter -source**

The list of 2 numbers, which specifies a coordinates of the source (or the center of the source if not point-like).

**Parameter -sourceBins**

The list of 2 values (float and int) to specify a source histogram range and number of bins. The first of them is a size of the plane in [mm] and the second is a number of source positions in each direction (positions are uniformly distributed among the source plane). 



**Parameter -n**

Number of generated events.


**Parameter -e**

Energy of generated $\gamma$-particles [kEv].


**Parameter -theta**

This parameter allows to set the $\theta_{min}$ angle [deg]. 
$\theta_{max}$ is always $180^o$ (parallel to the source-detector axis). It means that particles will be shot with angles $[\theta_{min}, 180^o]$.


**Parameter -sMac**

Path to the `mac`-script with additional specifications of the source. It allows to change type and shape of the source distribution.

<!-- ```
Example !TODO
```  -->


**Parameter -vis**

If this flag is used, the programs runs in visual mode[^2]. That is Geant4 graphical interface is opened with specified geometry setup.  


**Parameter -1d**

If this flag is used, the detector and mask are constructed in a singe-dimensional mode along x-axis  


### H-matrix calculation

```shell
mpirun -np n_cores ./cmd/mpi_gen_sim_matrix.cp output.root [optional arguments]
```

`n_cores` - is a number of parallel processes (number of cores used).

`output.root` is a name of the output root-file.

The most of the optional arguments are the same as for the [simulation](#optional-arguments), but some of them are not used or have different meaning.

##### Optional arguments

| Parameter                 | Default       | Type   |  Description   |   
| :------------------------: |:-------------:| :-------------:| :-------------|
|  `-det`            |    220:16:1.3    | list(float:int:float) | Detector parameters: detector-source:nFibres:fibre_width [mm/-/mm]
|  `-nlay`            |    4    | int  | Number of layers in detector)
|  `-mask`            |    31:170:70:20    | list(int:floats) | Mask parameters: order:mask-source:width/length:thickness [mm]
|  `-masktype`            |    standard    | string  | Mask type: \{`standard`, `round`, `pet`, `nowallpet`, `nowallpetcut`\}
|  `-cut`            |    31    | int  | Number of pixels in mask, (relevant only if masktype=nowallpetcut)
|  `-er`            |    0.0    | float  | Error in W rods size [mm], (relevant only if `masktype`=`nowallpetcut`)
|  `-source`            |    70:100    | list(float:int)  | Range and number of bins in the source plane
|  `-n`            |    1000    | int  | Number of generated events for each source position
|  `-e`            |    4400    | int  | Energy of particles [keV]
|  `-1d`            |    none    | flag  | Mask and detector are single-dimensional. The source position is shifted only along one dimension (`x`)

#### **Parameter -source**

The list of 2 values (float and int) to specify a source plane parameters. The first of them is a size of the plane in [mm] and the second is a number of source positions in each direction (positions are uniformly distributed among the source plane).[^3]


*All the rest parameters have the same meaning and usage as for [Simulation](#simulation)*

## Geometry

![Geometry](scripts/Geometry.png)

<!-- Footnotes -->
[^1]: The simulation is simpler than [one of JK](https://bragg.if.uj.edu.pl/gccbwiki/index.php/SiFi-CC/SiFi-CM_Geant4_Simulation) in a sense that it does not take into account a lot of details (electronics, fibers coupling etc.).

[^2]: Parameters of the graphical interface can be set in the file `vic_CC.mac` which is executed before the opening of the viewer and should be placed in the working directory.


[^3]: Usually it is convenient to set [`-sourceBins`](#parameter-sourcebins) parameter for the simulation to be the same as [`-source`](#parameter-source) parameter of system matrix for the further comparison of reconstructed image and original source histogram.

[^root]: Be aware that ROOT may not be using C++17 by default and one will need to compile it using option `-DCMAKE_CXX_STANDARD=17` 

<!-- Links -->
[sifi_link]: https://bragg.if.uj.edu.pl/gccbwiki/index.php/Main_Page "SiFi-CC"
[root_install]: https://root.cern/install/ "ROOT"