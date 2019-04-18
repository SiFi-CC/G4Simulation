
add_executable(test_cmd test_cmd.cpp)
target_link_libraries(test_cmd g4sim)

add_executable(gen_sim_matrix gen_sim_matrix.cpp)
target_link_libraries(gen_sim_matrix g4sim)

add_executable(simple_simulation simple_simulation.cpp)
target_link_libraries(simple_simulation g4sim)

add_executable(custom_simulation custom_simulation.cpp)
target_link_libraries(custom_simulation g4sim)
