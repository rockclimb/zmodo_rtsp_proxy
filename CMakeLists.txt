cmake_minimum_required(VERSION 2.8)
project(zmodo_rtsp_proxy)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(PkgConfig REQUIRED)
pkg_check_modules(live555 REQUIRED IMPORTED_TARGET live555)

# Add the sources
set(SOURCES read.cpp ZmodoMediaSubsession.cpp ZmodoSource.cpp
 ZmodoH264VideoStreamer.cpp ZmodoOnDemandRTSPServer.cpp)

add_executable(read read.cpp)
add_executable(ZmodoH264VideoStreamer ZmodoH264VideoStreamer.cpp ZmodoSource.cpp)
add_executable(ZmodoOnDemandRTSPServer ZmodoOnDemandRTSPServer.cpp ZmodoMediaSubsession.cpp ZmodoSource.cpp)

target_link_libraries(ZmodoH264VideoStreamer PUBLIC PkgConfig::live555)
target_link_libraries(ZmodoOnDemandRTSPServer PUBLIC PkgConfig::live555)

install(TARGETS ZmodoH264VideoStreamer ZmodoOnDemandRTSPServer DESTINATION bin)
install(PROGRAMS zmodo_set_time.py DESTINATION bin)
