# add_executable(gen_sim_matrix gen_sim_matrix.cpp) 
# target_link_libraries(gen_sim_matrix g4sim CmdLineArgs ${MPI_LIBRARIES})

add_executable(custom_simulation custom_simulation.cpp)
target_link_libraries(custom_simulation g4sim CmdLineArgs ${MPI_LIBRARIES})

# add_executable(mpi_gen_sim_matrix mpi_gen_sim_matrix.cpp)
# target_link_libraries(mpi_gen_sim_matrix g4sim CmdLineArgs ${MPI_LIBRARIES})

# #HypMed
# add_executable(custom_simulation_hypmed custom_simulation_hypmed.cpp)
# target_link_libraries(custom_simulation_hypmed g4sim CmdLineArgs ${MPI_LIBRARIES})

# add_executable(mpi_gen_sim_matrix_hypmed mpi_gen_sim_matrix_hypmed.cpp)
# target_link_libraries(mpi_gen_sim_matrix_hypmed g4sim CmdLineArgs ${MPI_LIBRARIES})