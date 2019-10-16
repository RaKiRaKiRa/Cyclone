set(LIB_SRC
    AsyncLogging.cc  
    CountDownLatch.cc 
    LogFile.cc  
    LogStream.cc  
    Thread.cc
    FileUtil.cc      
    Logging.cc
    Daemon.cc
)
add_definitions(-std=c++11)
add_library(Cyclone_base ${LIB_SRC})
target_link_libraries(Cyclone_base pthread rt)

set_target_properties(Cyclone_base PROPERTIES OUTPUT_NAME "Cyclone_base")

add_subdirectory(test)
