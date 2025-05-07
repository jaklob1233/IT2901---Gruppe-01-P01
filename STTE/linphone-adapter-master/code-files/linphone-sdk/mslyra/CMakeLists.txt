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

project(mslyra VERSION 1.0 LANGUAGES C)

set(CMAKE_CXX_FLAGS "-fpermissive")

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

include_directories(
        ${MEDIASTREAMER2_INCLUDE_DIRS}
        .
        ../../lyra/include
)

add_definitions(-DVERSION=\"${PROJECT_VERSION}\")

# install liblyra_shared shared lib
add_library(liblyra_shared SHARED IMPORTED)
set_target_properties(liblyra_shared PROPERTIES
        IMPORTED_LOCATION "../../lyra/lib/liblyra_shared.so"
        IMPORTED_NO_SONAME TRUE
        )

set(LIBS liblyra_shared ${MEDIASTREAMER2_LIBRARIES} ${ORTP_LIBRARIES} ${BCTOOLBOX_CORE_LIBRARIES})

set(SOURCE_FILES
        lyra_dec.cc
        lyra_enc.cc
        params.c
        )

set(MS2_PLUGINS_DIR "${MEDIASTREAMER2_PLUGINS_LOCATION}")

if (BUILD_SHARED_LIBS)
    add_library(mslyra MODULE ${SOURCE_FILES})
else ()
    add_library(mslyra STATIC ${SOURCE_FILES})
endif ()
target_link_libraries(mslyra ${LIBS})
if (BUILD_SHARED_LIBS)
    if (MSVC)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            install(FILES $<TARGET_PDB_FILE:mslyra>
                    DESTINATION ${CMAKE_INSTALL_BINDIR}
                    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
                    )
        endif ()
        set_target_properties(mslyra PROPERTIES PREFIX "lib")
    endif ()
endif ()

## install liblyra_shared
install(IMPORTED_RUNTIME_ARTIFACTS liblyra_shared
        LIBRARY
        )

## install lyra model coeffs
install(FILES
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/model_coeffs/lyra_config.binarypb
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/model_coeffs/lyragan.tflite
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/model_coeffs/quantizer.tflite
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/model_coeffs/soundstream_encoder.tflite
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/model_coeffs/test_playback.wav
        DESTINATION lib/lyra_model_coeffs
        )

## install lyra test data
install(FILES
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/A_eng_f1.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/000302.WAV
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample1_8kHz.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample1_16kHz.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample1_32kHz.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample1_48kHz.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample2_8kHz.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample2_16kHz.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample2_32kHz.wav
        /home/toor/projects/IST-201/linphone-adapter/lyra/lyra/testdata/sample2_48kHz.wav
        DESTINATION lib/lyra_testdata
        )

install(TARGETS mslyra
        RUNTIME DESTINATION ${MS2_PLUGINS_DIR}
        LIBRARY DESTINATION ${MS2_PLUGINS_DIR}
        ARCHIVE DESTINATION ${MS2_PLUGINS_DIR}
        FRAMEWORK DESTINATION Frameworks
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )

### test bin
#add_executable(mslyra-tester ${SOURCE_FILES})
#target_link_libraries(mslyra-tester ${LIBS})
#install(TARGETS mslyra-tester
#	RUNTIME DESTINATION ${MS2_PLUGINS_DIR}
#	LIBRARY DESTINATION ${MS2_PLUGINS_DIR}
#	ARCHIVE DESTINATION ${MS2_PLUGINS_DIR}
#	FRAMEWORK DESTINATION Frameworks
#	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
#)

# CPack settings
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
        "^${CMAKE_BINARY_DIR}"
        "/\\\\..+"
        )

include(CPack)
