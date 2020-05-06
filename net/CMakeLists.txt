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
    ServerWithHeartBeat.cc
    HttpStaticFile.cc
    Httpserver.cc
    HttpContext.cc
    HttpResponse.cc
    Connection.cc
    Buffer.cc
    Connector.cc
    Client.cc
)
include_directories(${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)
include_directories(${PROJECT_SOURCE_DIR}/base)


add_subdirectory(base)
add_library(Cyclone_net ${SRCS})
target_link_libraries(Cyclone_net Cyclone_base)


add_subdirectory(test)


