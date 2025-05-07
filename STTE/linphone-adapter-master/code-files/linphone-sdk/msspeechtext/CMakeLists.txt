############################################################################
# CMakeLists.txt
# Copyright (C) 2014-2023  Belledonne Communications, Grenoble France
#
############################################################################
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
############################################################################

cmake_minimum_required(VERSION 3.0)

project(msspeechtext VERSION 1.0)

#set(CMAKE_CXX_FLAGS "-fpermissive")
# speechtotext - server
set(CMAKE_CXX_FLAGS "-std=c++17 -pg -g3 -fpermissive")

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
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
    message(FATAL_ERROR "CURL not found! Please install libcurl-dev or provide the correct path.")
endif()

include_directories(
        ${MEDIASTREAMER2_INCLUDE_DIRS}
        .
)

add_definitions(-DVERSION=\"${PROJECT_VERSION}\")

# speechtotext - server
include(ExternalProject)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external)
ExternalProject_Add(NLOHMANN
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DJSON_BuildTests=OFF
)
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
add_subdirectory("speechtotext/server")
add_subdirectory("texttospeech/server")
add_subdirectory("mqtttransport/server")
set(SPEECH_TO_TEXT_SERVER_LIBS speechtotextServer texttospeechServer mqtttransportServer pistache pthread)

# speechtotext - client
add_subdirectory("speechtotext/client")
add_subdirectory("texttospeech/client")
add_subdirectory("mqtttransport/client")
set(SPEECH_TO_TEXT_CLIENT_LIBS speechtotextClient texttospeechClient mqtttransportClient)

# speechtotext - server and client
set(LIBS ${MEDIASTREAMER2_LIBRARIES} ${ORTP_LIBRARIES} ${BCTOOLBOX_CORE_LIBRARIES} ${SPEECH_TO_TEXT_SERVER_LIBS} ${SPEECH_TO_TEXT_CLIENT_LIBS})

set(SOURCE_FILES
        speechtext_dec.cc
        speechtext_enc.cc
        params.c
        central_logger.cc

        # speechtotext - server
        ${SPEECH_TO_TEXT_SERVER_SOURCE_FILES}
        )

set(MS2_PLUGINS_DIR "${MEDIASTREAMER2_PLUGINS_LOCATION}")

if (BUILD_SHARED_LIBS)
    add_library(msspeechtext MODULE ${SOURCE_FILES})
else ()
    add_library(msspeechtext STATIC ${SOURCE_FILES})
endif ()

#speechtotext/texttospeech/mqtttransport - server
add_dependencies(${PROJECT_NAME} NLOHMANN)

target_link_libraries(msspeechtext ${LIBS})
if (BUILD_SHARED_LIBS)
    if (MSVC)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            install(FILES $<TARGET_PDB_FILE:msspeechtext>
                    DESTINATION ${CMAKE_INSTALL_BINDIR}
                    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
                    )
        endif ()
        set_target_properties(msspeechtext PROPERTIES PREFIX "lib")
    endif ()
endif ()

install(TARGETS msspeechtext
        RUNTIME DESTINATION ${MS2_PLUGINS_DIR}
        LIBRARY DESTINATION ${MS2_PLUGINS_DIR}
        ARCHIVE DESTINATION ${MS2_PLUGINS_DIR}
        FRAMEWORK DESTINATION Frameworks
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
        "^${CMAKE_BINARY_DIR}"
        "/\\\\..+"
        )

include(CPack)
