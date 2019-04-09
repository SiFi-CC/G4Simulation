# G4Simulation

### Binaries

#### gen_sim_matrix
File `cmd/gen_sim_matrix.cpp` defines set of nested loops that
defines parameters of simulations that will be generated. All
data will be saved to single .root file separated into subdirectories
``` bash
cmake .. && make && ./cmd/gen_sim_matrix
```

### Scripts
Scripts for postprocessing data generated in simulations

#### extract_to_txt
Extracts data from root files generated usng `gen_sim_matrix`
```bash
python scripts/extract_to_txt.py ./build/simulation_results.root
```

#### extract_stats
Extracts data from root files generated usng `gen_sim_matrix`
```bash
python scripts/extract_stats.py ./build/simulation_results.root <some_name>
```
