cmake_minimum_required(VERSION 3.22)
project(msmelp VERSION 1.0.0)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/mediastreamer2/plugins")

include(GNUInstallDirs)

if (NOT CMAKE_INSTALL_RPATH AND CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})
    message(STATUS "Setting install rpath to ${CMAKE_INSTALL_RPATH}")
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(BcToolbox)
if (NOT BCTOOLBOX_FOUND)
    find_package(bctoolbox CONFIG REQUIRED)
endif ()
find_package(Ortp)
if (NOT ORTP_FOUND)
    find_package(ortp CONFIG REQUIRED)
endif ()
find_package(Mediastreamer2)
if (NOT MEDIASTREAMER2_FOUND)
    find_package(Mediastreamer2 CONFIG REQUIRED)
endif ()

add_definitions(-DVERSION=\"${PROJECT_VERSION}\")

set(MS2_PLUGINS_DIR "${MEDIASTREAMER2_PLUGINS_LOCATION}")

add_subdirectory(libs/melp)
add_subdirectory(sources)
