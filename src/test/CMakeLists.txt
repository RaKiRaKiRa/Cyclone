add_definitions(-std=c++11)
add_executable(loop_test loop_test.cc)
target_link_libraries(loop_test Cyclone_net)
