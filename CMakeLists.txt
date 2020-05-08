# /**********
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
# more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
# **********/
# Copyright (c) 2020 Andrew Ross. All rights reserved.

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
install(PROGRAMS zmodo_set_time DESTINATION bin)
