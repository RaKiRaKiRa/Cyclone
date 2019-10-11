add_definitions(-std=c++11)
add_executable(loop_test loop_test.cc)
target_link_libraries(loop_test Cyclone_net)

add_executable(TimeQueue_test TimeQueue_test.cc)
target_link_libraries(TimeQueue_test Cyclone_net Cyclone_base)

add_executable(Acceptor_test Acceptor_test.cc)
target_link_libraries(Acceptor_test Cyclone_net Cyclone_base)

add_executable(echo_test echo_multi.cc)
target_link_libraries(echo_test Cyclone_net Cyclone_base)

add_executable(echo_heart echo_heart.cc)
target_link_libraries(echo_heart Cyclone_net Cyclone_base)

add_executable(httpServer webServer.cc)
target_link_libraries(httpServer Cyclone_net Cyclone_base)
