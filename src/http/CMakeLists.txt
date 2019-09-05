set(http_SRCS
    httpStaticFile.cc
    httpserver.cc
    httpContext.cc
    httpResponse.cc
)

add_definitions(-std=c++11)
add_library(Cyclone_http ${http_SRCS})
target_link_libraries(Cyclone_net Cyclone_base)



