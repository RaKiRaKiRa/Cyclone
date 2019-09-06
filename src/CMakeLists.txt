set(SRCS
    Channel.cc
    EventLoop.cc
    IPoller.cc
    Poller.cc
    Epoller.cc
    Timer.cc
    TimeQueue.cc 
    EventLoopThread.cc
    Socket.cc
    SocketOpts.cc
    Acceptor.cc
    EventLoopThreadPool.cc
    Server.cc
    serverWithHeartBeat.cc
    httpStaticFile.cc
    httpserver.cc
    httpContext.cc
    httpResponse.cc
    Connection.cc
    Buffer.cc
)
add_definitions(-std=c++11)
include_directories(${PROJECT_SOURCE_DIR}/base)


add_subdirectory(base)
add_library(Cyclone_net ${SRCS})
target_link_libraries(Cyclone_net Cyclone_base)


add_subdirectory(test)


