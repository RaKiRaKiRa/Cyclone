set(SRCS
    Channel.cc
    IPoller.cc
    Poller.cc
    Epoller.cc
    EventLoop.cc
    #EventLoopThread.cc
    #EventLoopThreadPool.cc
    #HttpData.cc
    #Main.cc
    #Server.cc
    #ThreadPool.cc
    #Timer.cc
    #Util.cc
)
add_definitions(-std=c++11)
include_directories(${PROJECT_SOURCE_DIR}/base)


add_subdirectory(base)
add_library(Cyclone_net ${SRCS})
target_link_libraries(Cyclone_net Cyclone_base)


add_subdirectory(test)
